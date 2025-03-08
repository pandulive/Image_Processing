// C++ program for the above approach 
#include <iostream> 
#include <opencv2/opencv.hpp> 
using namespace cv; 
using namespace std; 
  
// Driver code 
int main(int argc, char** argv) 
{ 
    // Read the image file as 
    // imread("default.jpg"); 
    Mat image = imread("../image1.jpeg", IMREAD_COLOR); 
    // Mat gray, blurred, edges;
    cv::Mat img, sharpened, denoised, enhanced, upscaled;
    resize(image, img, Size(400,250), INTER_LINEAR);
    
    // Error Handling 
    if (img.empty()) { 
        cout << "Image File "
             << "Not Found" << endl; 
  
        // wait for any key press 
        cin.get(); 
        return -1; 
    } 
    Size sz = image.size();
    int imageWidth = sz.width;
    int imageHeight = sz.height;
    cout << "Width " << imageWidth << " Height " << imageHeight <<endl;
    // Step 1: Denoise the image (preserve edges)
    
    cv::fastNlMeansDenoisingColored(img, denoised, 1, 2, 7, 21);

    // Step 2: Apply sharpening to restore edges
    
    cv::Mat kernel = (cv::Mat_<float>(3,3) <<  
            0, -1,  0,  
            -1,  5, -1,  
            0, -1,  0);
    cv::filter2D(denoised, sharpened, -1, kernel);

    // Step 3: Apply CLAHE (Contrast Enhancement)
    std::vector<cv::Mat> channels(3);
    cv::split(sharpened, channels);
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(5.0); // Clip limit = 4.0
    clahe->apply(channels[0], channels[0]);
    clahe->apply(channels[1], channels[1]);
    clahe->apply(channels[2], channels[2]);

    cv::Mat final_image;
    cv::merge(channels, final_image);

    // Show results
    cv::imshow("Original Image", img);
    cv::imshow("Enhanced Image", final_image);

    cv::Mat gaussian, median, bilateral;

    // Apply different smoothing methods
    cv::GaussianBlur(final_image, gaussian, cv::Size(5, 5), 0);
    cv::medianBlur(final_image, median, 5);
    cv::bilateralFilter(final_image, bilateral, 9, 75, 75);
    
    // Show results
    // cv::imshow("Original", img);
    // cv::imshow("Gaussian Blur", gaussian);
    // cv::imshow("Median Blur", median);
    cv::imshow("Bilateral Filter", bilateral);

    cv::Mat darkened;
    final_image.convertTo(darkened, -1, 1, -50);
    cv::imshow("Darkened Image", darkened);

    cv::Mat gamma_corrected, gamma_bilateral;
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    double gamma = 0.5; // Lower value = darker image
    for (int i = 0; i < 256; i++)
        p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);

    cv::LUT(final_image, lookUpTable, gamma_corrected);
    cv::imshow("Gamma Corrected Image", gamma_corrected);

    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    // std::vector<cv::Mat> channels(3);
    cv::split(hsv, channels);

    // Reduce illuminance (V channel)
    channels[2] -= 3;  // Decrease brightness in V channel

    cv::merge(channels, hsv);
    cv::cvtColor(hsv, darkened, cv::COLOR_HSV2BGR);
    cv::imshow("Illuminance Reduced", darkened);


    cv::waitKey(0);
    return 0; 
} 