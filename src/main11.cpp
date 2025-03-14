// This is working the best of Brightness, Contrast and saturation control using V4L2.
// This code is tested on Ubuntu 22.04 with OpenCV 4.5.1 and V4L2.
// To compile this code, you can use this command:
// g++ -std=c++17 -o app main.cpp `pkg-config opencv4 --cflags --libs`
// To run this code, you can use this command:
// ./app
// Press 'w' to increase brightness, 's' to decrease brightness.
// Press 'e' to increase contrast, 'd' to decrease contrast.
// Press 'r' to increase saturation, 'f' to decrease saturation.
// Press 'q' to quit the program.
// You can adjust the brightness, contrast, and saturation of the camera in real-time.
// You can also see the current values of brightness, contrast, and saturation on the video.

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

// **Function to Set Brightness, Contrast, and Saturation Using V4L2**
void setCameraSettings(int brightness, int contrast, int saturation) {
    std::ostringstream command;
    command << "v4l2-ctl --set-ctrl=brightness=" << brightness
            << " --set-ctrl=contrast=" << contrast
            << " --set-ctrl=saturation=" << saturation;
    std::system(command.str().c_str());
    std::cout << "Set Brightness: " << brightness 
              << ", Contrast: " << contrast 
              << ", Saturation: " << saturation << " (V4L2)\n";
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
    int brightness = 0;   // Default Brightness (Range 0 - 15)
    int contrast = 13;    // Default Contrast (Range 0 - 30)
    int saturation = 9;  // Default Saturation (Range 0 - 60)

    // **Apply Initial Camera Settings**
    setCameraSettings(brightness, contrast, saturation);

    while (true) {
        cap >> frame;
        if (frame.empty()) continue;

        // **Display Brightness, Contrast, Saturation on Video**
        std::string text = "Brightness: " + std::to_string(brightness) +
                           " | Contrast: " + std::to_string(contrast) +
                           " | Saturation: " + std::to_string(saturation);
        cv::putText(frame, text, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);

        cv::imshow("Live Video - Adjust Settings", frame);

        // **Keyboard Controls**
        char key = cv::waitKey(1);
        if (key == 'q') break;
        if (key == 'w' && brightness < 15) brightness++;  // Increase Brightness
        if (key == 's' && brightness > -15) brightness--;   // Decrease Brightness
        if (key == 'e' && contrast < 30) contrast++;      // Increase Contrast
        if (key == 'd' && contrast > 0) contrast--;       // Decrease Contrast
        if (key == 'r' && saturation < 60) saturation++;  // Increase Saturation
        if (key == 'f' && saturation > 0) saturation--;   // Decrease Saturation

        // **Apply Settings to Camera**
        setCameraSettings(brightness, contrast, saturation);

        std::cout << text << std::endl;
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}
