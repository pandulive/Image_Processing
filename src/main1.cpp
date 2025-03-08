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
    Mat image = imread("../image.jpeg", IMREAD_COLOR); 
    Mat gray, blurred, edges;
  
    // Error Handling 
    if (image.empty()) { 
        cout << "Image File "
             << "Not Found" << endl; 
  
        // wait for any key press 
        cin.get(); 
        return -1; 
    } 
  
    Size sz = image.size();
    int imageWidth = sz.width;
    int imageHeight = sz.height;
    // Show Image inside a window with 
    // the name provided 
    cout << "Width " << imageWidth << " Height " << imageHeight <<endl;
    Mat resized_down;
    resize(image, resized_down, Size(450,800), INTER_LINEAR);
    // imshow("Window Name", image); 
    imshow("Size reduced", resized_down); 
  
    Mat sharpened;
    cv::Mat kernel = (cv::Mat_<float>(3,3) << 
        0, -1,  0, 
        -1,  5, -1, 
        0, -1,  0);

    cv::filter2D(resized_down, sharpened, -1, kernel);
    cv::imshow("Sharpened", sharpened);

    cv::Mat denoised;
    cv::fastNlMeansDenoisingColored(resized_down, denoised, 10, 10, 7, 21);
    cv::imshow("Denoised Image", denoised);

    // cv::Mat equalized;
    // cv::cvtColor(resized_down, gray, cv::COLOR_BGR2GRAY);

    // cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0); // Clip limit = 3.0
    // clahe->apply(gray, equalized);

    // cv::imshow("Equalized Image", equalized);

    std::vector<cv::Mat> channels(3);
    cv::split(resized_down, channels);  // Split into B, G, R channels

    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0); // Clip limit = 3.0
    clahe->apply(channels[0], channels[0]);
    clahe->apply(channels[1], channels[1]);
    clahe->apply(channels[2], channels[2]);

    cv::Mat enhanced;
    cv::merge(channels, enhanced);  // Merge back to BGR
    cv::imshow("Contrast Enhanced Color Image", enhanced);

    // // Convert to grayscale
    // cvtColor(resized_down, gray, cv::COLOR_BGR2GRAY);

    // // Apply Gaussian Blur
    // GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    // // Detect edges using Canny
    // Canny(blurred, edges, 10, 150);

    // // Show results
    // imshow("Grayscale", gray);
    // imshow("Blurred", blurred);
    // imshow("Edges", edges);

    // Wait for any keystroke 
    waitKey(0); 
    return 0; 
} 