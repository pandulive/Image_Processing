#include <opencv2/opencv.hpp>
#include <iostream>

double estimateBrightness(const cv::Mat& image) {
    cv::Scalar meanIntensity = cv::mean(image);
    return (meanIntensity[0] + meanIntensity[1] + meanIntensity[2]) / 3.0;
}

double estimateContrast(const cv::Mat& image) {
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::Scalar mean, stddev;
    cv::meanStdDev(gray, mean, stddev);
    return stddev[0];
}

double estimateSaturation(const cv::Mat& image) {
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
    std::vector<cv::Mat> hsvChannels;
    cv::split(hsv, hsvChannels);
    return cv::mean(hsvChannels[1])[0];
}

double estimateColorTemperature(const cv::Mat& image) {
    cv::Scalar avgRGB = cv::mean(image);
    double r = avgRGB[2], g = avgRGB[1], b = avgRGB[0];

    // Approximate color temperature (based on RGB ratio)
    double temp = 10000 * (r / (b + 1e-6));  // Avoid division by zero
    temp = std::min(std::max(temp, 1000.0), 15000.0);  // Keep within range
    return temp;
}

void adjustWhiteBalance(cv::Mat& image, double targetTemp = 6500) {
    double colorTemp = estimateColorTemperature(image);
    double scaleFactor = targetTemp / (colorTemp + 1e-6);

    std::vector<cv::Mat> bgrChannels;
    cv::split(image, bgrChannels);
    
    bgrChannels[0] *= scaleFactor; // Blue
    bgrChannels[2] /= scaleFactor; // Red

    cv::merge(bgrChannels, image);
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
    bool autoWB = true;

    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        // **Estimate Metrics**
        double brightness = estimateBrightness(frame);
        double contrast = estimateContrast(frame);
        double saturation = estimateSaturation(frame);
        double colorTemperature = estimateColorTemperature(frame);

        // **Auto White Balance Adjustment**
        if (autoWB) adjustWhiteBalance(frame, 6500);

        // **Display Metrics on Video**
        std::string text = "Brightness: " + std::to_string(brightness) +
                           " | Contrast: " + std::to_string(contrast) +
                           " | Saturation: " + std::to_string(saturation) +
                           " | Temp: " + std::to_string(colorTemperature) + "K";

        cv::putText(frame, text, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Live Video - Auto White Balance: " + std::string(autoWB ? "ON" : "OFF"), frame);

        // **Keyboard Controls**
        char key = cv::waitKey(1);
        if (key == 'q') break;
        if (key == 't') autoWB = !autoWB; // Toggle Auto White Balance

        std::cout << text << std::endl;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
