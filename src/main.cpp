#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat image = imread("../image2.jpeg", cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }
    cv::Mat img;
    resize(image, img, cv::Size(400,250), cv::INTER_LINEAR);

    // Convert to LAB color space (L = lightness, A/B = color)
    cv::Mat lab;
    cv::cvtColor(img, lab, cv::COLOR_BGR2Lab);

    // Split LAB channels
    std::vector<cv::Mat> lab_channels;
    cv::split(lab, lab_channels);

    // Process only the Luminance (L) channel
    for (int y = 0; y < lab_channels[0].rows; y++) {
        for (int x = 0; x < lab_channels[0].cols; x++) {
            uchar& L = lab_channels[0].at<uchar>(y, x);
            if (L > 200) { // If pixel is too bright
                float reduction_factor = 0.75 + 0.25 * ((255 - L) / 55.0); // Dynamic adjustment
                L = cv::saturate_cast<uchar>(L * reduction_factor); // Reduce intensity smoothly
                
            }
        }
        
    }

    // Merge LAB channels and convert back to BGR
    cv::merge(lab_channels, lab);
    cv::Mat result;
    cv::cvtColor(lab, result, cv::COLOR_Lab2BGR);

    // Show images
    cv::imshow("Original Image", img);
    cv::imshow("Flash Reduced Image", result);
    
    
    cv::waitKey(0);

    return 0;
}


