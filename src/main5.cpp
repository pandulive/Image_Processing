#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Open webcam
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open webcam!" << std::endl;
        return -1;
    }

    // Set resolution (optional)
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    cv::Mat frame, lab, enhanced, hsv;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cerr << "Warning: Empty frame! Skipping..." << std::endl;
            continue;
        }

        // **Step 1: Convert to LAB color space for Contrast Enhancement**
        cv::cvtColor(frame, lab, cv::COLOR_BGR2Lab);
        std::vector<cv::Mat> lab_channels;
        cv::split(lab, lab_channels);

        // **Apply CLAHE to L-channel (prevent over-processing)**
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0); // Lower clip limit = 2.0
        clahe->apply(lab_channels[0], lab_channels[0]);

        // Merge LAB and convert back to BGR
        cv::merge(lab_channels, lab);
        cv::cvtColor(lab, enhanced, cv::COLOR_Lab2BGR);

        // **Step 2: Convert to HSV for Saturation Boost**
        cv::cvtColor(enhanced, hsv, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsv_channels;
        cv::split(hsv, hsv_channels);

        // **Increase Saturation (Avoid Over-Saturation)**
        hsv_channels[1] = hsv_channels[1] * 1.3; // Increase saturation by 30%
        cv::merge(hsv_channels, hsv);
        cv::cvtColor(hsv, enhanced, cv::COLOR_HSV2BGR);

        // **Step 3: Apply a slight Gaussian Blur for smoothness**
        // cv::GaussianBlur(enhanced, enhanced, cv::Size(3, 3), 0);

        // Show video stream
        cv::imshow("Original Video", frame);
        cv::imshow("Enhanced Color Video", enhanced);

        // Exit on 'q' key press
        if (cv::waitKey(1) == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
