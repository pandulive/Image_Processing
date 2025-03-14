// this code sets the color temperature to a fixed value of 4500K. and sets the read value to the camera.
#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

// **Better Color Temperature Estimation (1000K - 10000K)**
double estimateColorTemperature(const cv::Mat& image) {
    cv::Scalar avgRGB = cv::mean(image);
    double r = avgRGB[2], g = avgRGB[1], b = avgRGB[0];

    // **Improved Nonlinear Scaling Formula for Color Temperature**
    double temp;
    double ratio = r / (b + 1e-6);  // Avoid division by zero

    if (ratio > 1.0) {
        temp = 4500.0 * std::pow(ratio, 1.2);  // Warmer colors
    } else {
        temp = 4500.0 / std::pow(1.0 / ratio, 1.2);  // Cooler colors
    }

    temp = std::min(std::max(temp, 1000.0), 10000.0);  // Clamp between 1000K - 10000K
    return temp;
}

// **Function to Set White Balance Using V4L2**
void setWhiteBalanceV4L2(int wb_value) {
    std::ostringstream command;
    command << "v4l2-ctl --set-ctrl=white_balance_temperature=" << wb_value;
    std::system(command.str().c_str());
    std::cout << "Set White Balance to: " << wb_value << "K (V4L2)\n";
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

    cv::Mat frame;
    bool autoWB = true;
    int manualWB = 4500;  // Default white balance temperature

    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        // **Estimate Corrected Color Temperature (1000K - 10000K)**
        double colorTemperature = estimateColorTemperature(frame);

        // **Apply Manual White Balance When AWB is OFF**
        if (!autoWB) {
            setWhiteBalanceV4L2(manualWB);
        }

        // **Display Color Temperature & AWB Status**
        std::string text = "Color Temp: " + std::to_string(colorTemperature) + "K | AWB: " + (autoWB ? "ON" : "OFF");
        cv::putText(frame, text, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Live Video - Auto WB: " + std::string(autoWB ? "ON" : "OFF"), frame);

        // **Keyboard Controls**
        char key = cv::waitKey(1);
        if (key == 'q') break;
        if (key == 't') { 
            autoWB = !autoWB;  // Toggle AWB
            if (!autoWB) {
                manualWB = static_cast<int>(colorTemperature); // Read current value
                manualWB = std::min(std::max(manualWB, 1000), 10000); // Clamp to 1000K - 10000K
                setWhiteBalanceV4L2(manualWB); // Apply to camera
            } else {
                setWhiteBalanceV4L2(4500);  // Default to 4500K when switching AWB ON
            }
        }

        std::cout << text << std::endl;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
