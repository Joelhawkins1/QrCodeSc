#include <opencv2/opencv.hpp> // OpenCv Used for capturing video detecting QR codes and decoding them
#include <windows.h>
#include <shellapi.h> // uses the windows command shell to open a url in your browser
#include <string> 
#include <iostream>
#include <QMainWindow>  // Main class for QTGraphics
#include <QTimer>       // Timer class
#include <QLabel>       // To show the video feed
#include <QLineEdit>    // To show the decoded QR text

using namespace cv;
using namespace std;


int main()
{
	Mat frame; // creats the variable frame using the "Mat" class holds a picture
	VideoCapture cap; // Initalizing VideoCapture Class creating a object named cap.
	QRCodeDetector decode; // Initalizing 

	cap.open(0, cv::CAP_DSHOW); // Openining the systems defualt camera 
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640); // Setting video resolution
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

	if (!cap.isOpened()) // Checks to see if camera is opened if not gives a error message
	{
		cout << "ERROR! Unable to open camera\n";
		return -1;
	}

	for(;;) // creating a infinte loop
	{
		cap.read(frame); // calls the videocapture object reads it and stores it in the frame variable created using the mat class.

		if (frame.empty()) // Testing to see if the image pulled is there.
		{
			cout << "ERROR! Blank image";
			break;
		}

		while (true) {
			cap >> frame;
			if (frame.empty()) break;

			Mat points, straight_qrcode;
			string data = decode.detectAndDecode(frame, points, straight_qrcode);
				if (!data.empty()) {
					cout << "QR Code Detected: " << data << endl;
					ShellExecuteA(NULL, "open", data.c_str(), NULL, NULL, SW_SHOWNORMAL); // Uses the windows shell to open the url with your systems browser.

					for (int i = 0; i < 4; i++) {
						cv::line(frame, points.at<cv::Point2f>(i),
							points.at<cv::Point2f>((i + 1) % 4),
							cv::Scalar(0, 255, 0), 3);
					}
				}

			cv::imshow("Camera Feed", frame);
			if (cv::waitKey(1) == 27) break; // Press 'Esc' to exit
		}
	}
	return 0;
}

/*
	Refrences.

	Windows.h https://learn.microsoft.com/en-us/windows/win32/shell/launch
	VideoCapture Documentation https://docs.opencv.org/4.x/d8/dfe/classcv_1_1VideoCapture.html#a57c0e81e83e60f36c83027dc2a188e80
	QRCodeDetector Documnetation https://docs.opencv.org/4.x/de/dc3/classcv_1_1QRCodeDetector.html#a397753e5eda6c54f10671e9013682bd7

*/
