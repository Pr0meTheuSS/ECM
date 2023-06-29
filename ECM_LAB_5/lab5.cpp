#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <ctime>
#include <chrono>

using namespace cv;

int main(int argc, char **argv) {
    Mat frame;
    VideoCapture cap;
    cap.open(0);

    // Default resolutions of the frame are obtained.The default resolutions are system dependent.
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    
    // Define the codec and create VideoWriter object.The output is stored in 'out.avi' file.
    VideoWriter video("out.avi", cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));
    
    std::chrono::time_point<std::chrono::system_clock> begin, end;

    begin = std::chrono::system_clock::now();
    
    while(cap.isOpened() && cap.read(frame)) {
        if (waitKey(1)==27) break;
        
        // Frame processing
        // GaussianBlur( frame, frame, Size(3, 3), 0, 0, BORDER_DEFAULT );
        Laplacian(frame, frame, CV_8U, 3, 1, BORDER_DEFAULT);
        
        // Calc time from prev frame to now
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_time = (end - begin);
        std::string fpsInfo = "FPS: " + std::to_string(1.0 / elapsed_time.count());
        begin = end;
        video.write(frame);

        // Put FPS data on frame
        putText(frame, 
                fpsInfo, 
                Point(frame.cols / 10, frame.rows / 10), 
                FONT_HERSHEY_COMPLEX, 
                0.5, 
                Scalar(255,255,255));

        // load frame to video stream
        imshow("lab5", frame);
    }
    cap.release();
    destroyAllWindows();
}

