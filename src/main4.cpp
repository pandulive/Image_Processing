#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Open webcam (0 = default camera)
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Cannot open webcam!" << std::endl;
        return -1;
    }

    // // Set resolution to 640x480
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    cv::Mat frame;
    while (true) {
        cap >> frame;  // Capture frame
        if (frame.empty()) break;

        // Convert to LAB color space
        cv::Mat lab;
        cv::cvtColor(frame, lab, cv::COLOR_BGR2Lab);

        // Split LAB channels
        std::vector<cv::Mat> lab_channels;
        cv::split(lab, lab_channels);

        // Reduce brightness in bright spots
        for (int y = 0; y < lab_channels[0].rows; y++) {
            for (int x = 0; x < lab_channels[0].cols; x++) {
                uchar& L = lab_channels[0].at<uchar>(y, x);
                if (L > 200) {  // If pixel is too bright
                    L = cv::saturate_cast<uchar>(L * 0.7);  // Reduce intensity by 30%
                }
            }
        }

        // Apply sharpening
        cv::Mat sharpened;
        cv::Mat kernel = (cv::Mat_<float>(3,3) <<  
            0, -1,  0,  
           -1,  5, -1,  
            0, -1,  0);
        cv::filter2D(lab_channels[0], sharpened, -1, kernel);

        // Blend sharpened result with brightness-reduced frame
        for (int y = 0; y < lab_channels[0].rows; y++) {
            for (int x = 0; x < lab_channels[0].cols; x++) {
                uchar& L = lab_channels[0].at<uchar>(y, x);
                L = cv::saturate_cast<uchar>(0.6 * L + 0.4 * sharpened.at<uchar>(y, x));  // Blend
            }
        }

        // Merge LAB channels and convert back to BGR
        cv::merge(lab_channels, lab);
        cv::Mat result;
        cv::cvtColor(lab, result, cv::COLOR_Lab2BGR);

        // Show video stream
        cv::imshow("Original Video", frame);
        cv::imshow("Flash Reduced Video", result);

        // Exit on 'q' key press
        if (cv::waitKey(1) == 'q') break;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}