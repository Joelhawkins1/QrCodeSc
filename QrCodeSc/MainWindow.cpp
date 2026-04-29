#include "MainWindow.h"
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <QImage>
#include <QPixmap>
#include <shellapi.h>
#include <QMessageBox>
#include <Qpainter.h>
#include <Qbitmap.h>

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent) // constructor
{ 
    ui.setupUi(this);
    // ui.lbl_video->setMask(QRegion(0, 0, ui.lbl_video->width(), ui.lbl_video->height(), QRegion::Rectangle));
    ui.txt_qrData->hide();
    this->showMaximized(); // Tells Qt to open in full screen
    this->setWindowTitle("");

    connect(ui.btn_openUrl, &QPushButton::clicked, this, &MainWindow::on_btn_openUrl_clicked);

    // -*- Initializing the camera -*-
    cap.open(0, cv::CAP_DSHOW); // Defualt camera is held a the first index 0
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); // setting resolution

    // Setup the timer to act as your "infinite loop"
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(33); // roughly 30 FPS
}
void MainWindow::resizeEvent(QResizeEvent* event) {
     QMainWindow::resizeEvent(event);

    // Create a rounded rectangle mask // This gives the video output curved edges
     if (ui.lbl_video->width() > 0 && ui.lbl_video->height() > 0) 
     {
         QBitmap mask(ui.lbl_video->size());
         mask.fill(Qt::color0);
         QPainter painter(&mask);
         painter.setRenderHint(QPainter::Antialiasing);
         painter.setBrush(Qt::black);

         // 50, 50 is quite a large radius; 20-30 is usually standard
         painter.drawRoundedRect(ui.lbl_video->rect(), 50, 50);
         ui.lbl_video->setMask(mask);
     }
}

void MainWindow::updateFrame() 
{
    cv::Mat frame, points, straight_qrcode;
    cap >> frame; 

    if (!frame.empty()) 
    {
        // Checks to see if camera is working
        std::string data = decoder.detectAndDecode(frame, points, straight_qrcode);

        if (!data.empty()) 
        {
            currentUrl = data;

            ui.txt_qrData->setText(QString("<a href='%1' style='color: #0000FF; text-decoration: underline;'>%1</a>").arg(QString::fromStdString(data)));
            ui.txt_qrData->show(); // Show it when data is found

            // ui.txt_qrData->hide(); // Hide it when no QR is visible
        }
    }

        // -*- Convert MAT (OpenCV) To QIMAGE (QT) -*-
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        QImage qimg((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

        // -*- Dsiplay in MainWindow.UI -*- 
        ui.lbl_video->setPixmap(QPixmap::fromImage(qimg).scaled(ui.lbl_video->size(), Qt::KeepAspectRatio));
        // Use hardcoded 640x480 to see if it appears
        // ui.lbl_video->setPixmap(QPixmap::fromImage(qimg).scaled(640, 480, Qt::KeepAspectRatio));

}

void MainWindow::on_btn_openUrl_clicked()
{
    if (!currentUrl.empty()) {
        // This opens the URL in the default browser
        ShellExecuteA(NULL, "open", currentUrl.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
    else {
        // This will tell you if the button works but the URL is empty
        QMessageBox::warning(this, "Error", "No URL detected yet!");
    }
}

/*
	Refrences.

	Windows.h https://learn.microsoft.com/en-us/windows/win32/shell/launch
	VideoCapture Documentation https://docs.opencv.org/4.x/d8/dfe/classcv_1_1VideoCapture.html#a57c0e81e83e60f36c83027dc2a188e80
	QRCodeDetector Documnetation https://docs.opencv.org/4.x/de/dc3/classcv_1_1QRCodeDetector.html#a397753e5eda6c54f10671e9013682bd7

*/