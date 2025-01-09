
// OpenCV C++ Program to create a single colored blank image
// Last Updated : 19 Dec, 2022
// The following is the explanation to the C++ code to create a single colored blank image in C++ using the tool OpenCV.

// Things to know: 

// (1) The code will only compile in Linux environment.

// (2) To run in windows, please use the file: ‘blank.o’ and run it in cmd. However if it does not run (problem in system architecture) then compile it in windows by making suitable and obvious changes to the code like: Use <iostream.h> in place of <iostream>.

// (3) Compile command: g++ -w blank.cpp -o blank `pkg-config –libs opencv`

// (4) Run command: ./article

// Before you run the code, please make sure that you have OpenCV installed on your system.

// Code Snippet:

// Title: Create a coloured image in C++ using OpenCV.

// highgui - an easy-to-use interface to 
// video capturing, image and video codecs,
// as well as simple UI capabilities.
#include <iostream> 
#include "opencv2/highgui.hpp"


// Namespace where all the C++ OpenCV 
// functionality resides. 
using namespace cv;

// For basic input / output operations. 
// Else use macro 'std::' everywhere.
using namespace std;

int main()
{
    // To create an image
    // CV_8UC3 depicts : (3 channels,8 bit image depth
    // Height  = 500 pixels, Width = 1000 pixels
    // (0, 0, 100) assigned for Blue, Green and Red 
    //             plane respectively. 
    // So the image will appear red as the red 
    // component is set to 100.
    Mat img(500, 1000, CV_8UC3, Scalar(0,0,255));
    
    // check whether the image is loaded or not
    if (img.empty()) 
    {
        cout << "\n Image not created. You"
                     " have done something wrong. \n";
        return -1;    // Unsuccessful.
    }

    // first argument: name of the window
    // second argument: flag- types: 
    // WINDOW_NORMAL If this is set, the user can 
    //               resize the window.
    // WINDOW_AUTOSIZE If this is set, the window size
    //                is automatically adjusted to fit 
    //                the displayed image, and you cannot
    //                change the window size manually.
    // WINDOW_OPENGL  If this is set, the window will be
    //                created with OpenGL support.
    namedWindow("First work", WINDOW_AUTOSIZE);

    // first argument: name of the window
    // second argument: image to be shown(Mat object)
    imshow("First work", img);
    
    waitKey(0); //wait infinite time for a keypress

    // destroy the window with the name, "MyWindow"
    destroyWindow("First work");            

    return 0;
}