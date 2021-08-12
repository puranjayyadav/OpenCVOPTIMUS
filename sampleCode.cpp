#include "opencv2/opencv.hpp"
#include "iostream"
#include "opencv2/highgui.hpp"
#include<chrono>
#include<ctime>
using namespace cv;
using namespace std;

Mat frame,Matrix,framePers,frameGray, frameThresh, frameEdge,frameFinal, frameFinalDuplicate;
Mat ROILane;
int LeftLanePos, RightLanePos;
VideoCapture camera(0, CAP_DSHOW);
vector<int> HistogramLane;
Point2f Source[] = { Point2f(40,145),Point2f(360,145),Point2f(10,195),Point2f(390,195) };
Point2f Destination[] = { Point2f(100,0),Point2f(280,0),Point2f(100,240),Point2f(280,240) };

void setup(int argc, char** argv, VideoCapture &camera) {
    camera.set(CAP_PROP_FRAME_WIDTH, ("-w", argc, argv, 1280));
    camera.set(CAP_PROP_FRAME_HEIGHT, ("-h", argc, argv, 720));
    camera.set(CAP_PROP_BRIGHTNESS, ("-br", argc, argv, 50));
    camera.set(CAP_PROP_CONTRAST, ("-co", argc, argv, 50));
    camera.set(CAP_PROP_SATURATION, ("-sa", argc, argv, 50));
    camera.set(CAP_PROP_GAIN, ("-g", argc, argv, 50));
    camera.set(CAP_PROP_FPS, ("-fps", argc, argv, 30));
}
void Perspective() {
    line(frame, Source[0], Source[1], Scalar(0, 0, 255), 2);
    line(frame, Source[1], Source[3], Scalar(0, 0, 255), 2);
    line(frame, Source[3], Source[2], Scalar(0, 0, 255), 2);
    line(frame, Source[2], Source[0], Scalar(0, 0, 255), 2);

   Matrix= getPerspectiveTransform(Source, Destination);
    warpPerspective(frame, framePers, Matrix, Size(400, 240));
}
void Histogram() {
    HistogramLane.resize(400);
    HistogramLane.clear();

    for (int i = 0;i < 400;i++) {
        ROILane = frameFinal(Rect(i, 140, 1, 100));
        divide(255, ROILane, ROILane);
        HistogramLane.push_back((int)(sum(ROILane)[0]));
                        
    }
}

void Threshold() {
    cvtColor(framePers, frameGray, COLOR_RGB2GRAY);
    inRange(frameGray, 200, 250, frameThresh);
    Canny(frameGray, frameEdge, 100, 500, 3, false);
    add(frameThresh, frameEdge, frameFinal);
    cvtColor(frameFinal, frameFinalDuplicate, COLOR_RGB2BGR);
}

void LaneFinder() {
    vector<int>:: iterator LeftPtr;
    LeftPtr = max_element(HistogramLane.begin(), HistogramLane.begin() + 150);
    LeftLanePos = distance(HistogramLane.begin(), LeftPtr);

    vector<int>:: iterator RightPtr;
    RightPtr = max_element(HistogramLane.begin() + 250, HistogramLane.end());
    RightLanePos = distance(HistogramLane.begin(), RightPtr);

    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255, 0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0, 255, 0), 2);

}





void Capture() {
    camera.grab();
    camera.retrieve(frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);

}
int main(int argc, char** argv) {
    
    // open the first webcam plugged in the computer
    setup(argc, argv, camera);
    cout << "connectiing to the camera" << endl;
    if (!camera.isOpened()) {
        cout<< "ERROR: Could not open camera" <<endl;
        return -1;
    }
    while (1) {
        auto start = std::chrono::system_clock::now();
        Capture();
        Perspective();
        Threshold();
        Histogram();
        LaneFinder();
        namedWindow("BGR", WINDOW_KEEPRATIO);
        moveWindow("BGR", 0, 100);
        resizeWindow("BGR", 640, 480);
        imshow("BGR", frame);

        namedWindow("BGR", WINDOW_KEEPRATIO);
        moveWindow("BGR", 640, 100);
        resizeWindow("BGR", 640, 480);
        imshow("BGR", framePers);

        namedWindow("Gray", WINDOW_KEEPRATIO);
        moveWindow("Gray", 1280, 100);
        resizeWindow("Gray", 640, 480);
        imshow("Gray", frameFinal);
      
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        float t = elapsed_seconds.count();
        int FPS = 1 / t;
        cout << "FPS=" << FPS<<endl;
      //  imshow("frame", frame);
        waitKey(1);
    }
    
    return 0;

}