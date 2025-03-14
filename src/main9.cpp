#include <opencv2/opencv.hpp>
#include <iostream>

double estimateBrightness(const cv::Mat& image) {
    return cv::mean(image)[0];  // Average intensity
}

double estimateContrast(const cv::Mat& image) {
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::Scalar mean, stddev;
    cv::meanStdDev(gray, mean, stddev);
    return stddev[0];  // Standard deviation as contrast measure
}

double estimateSaturation(const cv::Mat& image) {
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    std::vector<cv::Mat> hsvChannels;
    cv::split(hsv, hsvChannels);
    return cv::mean(hsvChannels[1])[0];  // Mean of saturation channel
}

double estimateColorTemperature(const cv::Mat& image) {
    cv::Scalar avgRGB = cv::mean(image);
    double r = avgRGB[2], g = avgRGB[1], b = avgRGB[0];

    // Approximate color temperature (based on RGB ratio)
    double temp = 10000 * (r / (b + 1e-6));  // Avoid division by zero
    temp = std::min(std::max(temp, 1000.0), 15000.0);  // Keep within reasonable range
    return temp;
}

void applySettingsToCamera(cv::VideoCapture& cap, double brightness, double contrast, double saturation, double colorTemp) {
    cap.set(cv::CAP_PROP_BRIGHTNESS, brightness / 255.0);  // Normalize brightness to camera scale
    cap.set(cv::CAP_PROP_CONTRAST, contrast / 128.0);  // Normalize contrast
    cap.set(cv::CAP_PROP_SATURATION, saturation / 128.0);  // Normalize saturation

    // Adjust White Balance (if camera supports manual WB control)
    if (colorTemp < 5000) {
        cap.set(cv::CAP_PROP_WHITE_BALANCE_BLUE_U, 0.5);  // Cooler adjustment
        cap.set(cv::CAP_PROP_WHITE_BALANCE_RED_V, 1.5);  
    } else {
        cap.set(cv::CAP_PROP_WHITE_BALANCE_BLUE_U, 1.5);  // Warmer adjustment
        cap.set(cv::CAP_PROP_WHITE_BALANCE_RED_V, 0.5);  
    }
}

int main() {
    // Open USB Camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open webcam!" << std::endl;
        return -1;
    }

    // **Set Camera Resolution**
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    cv::Mat frame;
    bool autoAdjust = false;

    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        // **Estimate Metrics**
        double brightness = estimateBrightness(frame);
        double contrast = estimateContrast(frame);
        double saturation = estimateSaturation(frame);
        double colorTemperature = estimateColorTemperature(frame);

        // **Apply Settings to Camera (if enabled)**
        if (autoAdjust) {
            applySettingsToCamera(cap, brightness, contrast, saturation, colorTemperature);
        }

        // **Display Metrics on Video**
        std::string text = "Brightness: " + std::to_string(brightness) +
                           " | Contrast: " + std::to_string(contrast) +
                           " | Saturation: " + std::to_string(saturation) +
                           " | Temp: " + std::to_string(colorTemperature) + "K";

        cv::putText(frame, text, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Live Video - Auto Adjust: " + std::string(autoAdjust ? "ON" : "OFF"), frame);

        // **Keyboard Controls**
        char key = cv::waitKey(1);
        if (key == 'q') break;
        if (key == 'a') autoAdjust = !autoAdjust; // Toggle Auto Adjustment

        std::cout << text << std::endl;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
