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
    Mat image = imread("../image.jpeg", 
                       IMREAD_COLOR); 
  
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
    imshow("Window Name", image); 
    imshow("Size reduced", resized_down); 
  
    // Wait for any keystroke 
    waitKey(0); 
    return 0; 
} 