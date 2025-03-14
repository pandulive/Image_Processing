#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <regex>

// **Function to Read V4L2 Camera Settings**
int getCameraSetting(const std::string& setting_name) {
    std::ostringstream command;
    command << "v4l2-ctl --get-ctrl=" << setting_name;
    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to execute v4l2-ctl\n";
        return -1;
    }

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    std::smatch match;
    std::regex num_regex(R"(\d+)");
    if (std::regex_search(result, match, num_regex)) {
        try {
            return std::stoi(match.str());
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid number format for " << setting_name << "\n";
            return -1;
        }
    }

    std::cerr << "Error: Could not extract numeric value for " << setting_name << "\n";
    return -1;
}

// **Function to Set Camera Controls Using V4L2**
void setCameraSettings(int brightness, int contrast, int saturation, int wb_value) {
    static int last_brightness = -1, last_contrast = -1, last_saturation = -1, last_wb = -1;
    
    if (brightness == last_brightness && contrast == last_contrast && 
        saturation == last_saturation && wb_value == last_wb) {
        return;  // Skip if no changes
    }

    std::ostringstream command;
    command << "v4l2-ctl --set-ctrl=brightness=" << brightness
            << " --set-ctrl=contrast=" << contrast
            << " --set-ctrl=saturation=" << saturation
            << " --set-ctrl=white_balance_temperature=" << wb_value;
    std::system(command.str().c_str());

    last_brightness = brightness;
    last_contrast = contrast;
    last_saturation = saturation;
    last_wb = wb_value;

    std::cout << "Updated Settings -> Brightness: " << brightness 
              << ", Contrast: " << contrast 
              << ", Saturation: " << saturation 
              << ", White Balance: " << wb_value << "K\n";
}

// **Function to Estimate Color Temperature (1000K - 10000K)**
double estimateColorTemperature(const cv::Mat& image) {
    cv::Scalar avgRGB = cv::mean(image);
    double r = avgRGB[2], g = avgRGB[1], b = avgRGB[0];

    double ratio = r / (b + 1e-6);  // Avoid division by zero
    double temp = (ratio > 1.0) ? (4500.0 * std::pow(ratio, 1.2)) : (4500.0 / std::pow(1.0 / ratio, 1.2));

    return std::round(std::min(std::max(temp, 1000.0), 10000.0));  // Clamp & round to nearest integer
}

// **Function to Gradually Adjust White Balance**
int smoothWhiteBalance(int currentWB, int targetWB, double step = 0.1) {
    return static_cast<int>(currentWB + step * (targetWB - currentWB));
}

int main() {
    // **Open USB Camera**
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open webcam!" << std::endl;
        return -1;
    }

    // **Set Camera Resolution**
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    // **Read Initial Camera Settings**
    int brightness = getCameraSetting("brightness");
    int contrast = getCameraSetting("contrast");
    int saturation = getCameraSetting("saturation");
    int whiteBalance = getCameraSetting("white_balance_temperature");
    int lastRecordedWB = whiteBalance; // Store last WB when AWB was OFF

    brightness = std::max(0, std::min(15, brightness));
    contrast = std::max(0, std::min(30, contrast));
    saturation = std::max(0, std::min(60, saturation));
    whiteBalance = std::max(1000, std::min(10000, whiteBalance));

    cv::Mat frame;
    bool autoWB = true;

    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        // **Estimate Corrected Color Temperature (1000K - 10000K)**
        double colorTemperature = estimateColorTemperature(frame);

        // **If AWB is OFF, Smoothly Adjust White Balance Instead of Jumping**
        if (!autoWB) {
            int targetWB = static_cast<int>(colorTemperature);
            targetWB = std::min(std::max(targetWB, 1000), 10000);
            whiteBalance = smoothWhiteBalance(whiteBalance, targetWB, 0.05); // 5% smooth transition
            lastRecordedWB = whiteBalance;  // Store last WB used in AWB OFF mode
        }

        // **Apply Settings to Camera**
        setCameraSettings(brightness, contrast, saturation, whiteBalance);

        // **Display Camera Settings on Video**
        std::string text = "Brightness: " + std::to_string(brightness) +
                           " | Contrast: " + std::to_string(contrast) +
                           " | Saturation: " + std::to_string(saturation) +
                           " | WB: " + std::to_string(whiteBalance) + "K | AWB: " + (autoWB ? "ON" : "OFF");
        cv::putText(frame, text, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Live Video - Camera Controls", frame);

        // **Keyboard Controls**
        char key = cv::waitKey(1);
        if (key == 'q') break;
        if (key == 'w' && brightness < 15) brightness++;  
        if (key == 's' && brightness > 0) brightness--;   
        if (key == 'e' && contrast < 30) contrast++;      
        if (key == 'd' && contrast > 0) contrast--;       
        if (key == 'r' && saturation < 60) saturation++;  
        if (key == 'f' && saturation > 0) saturation--;   
        if (key == 't') { 
            autoWB = !autoWB;
            if (autoWB) {
                whiteBalance = lastRecordedWB;  // Use the last recorded WB when turning AWB ON
            } else {
                whiteBalance = static_cast<int>(colorTemperature);
                whiteBalance = std::min(std::max(whiteBalance, 1000), 10000);
            }
        }

        std::cout << text << std::endl;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
