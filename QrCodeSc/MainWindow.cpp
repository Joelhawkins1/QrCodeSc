#include "MainWindow.h"
#include <opencv2/opencv.hpp>
#include <windows.h> // required for shellapi
#include <shellapi.h> // uses windows power shell to open url
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <Qpainter.h>
#include <QPainterPath>

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent) // constructor
{ 
    ui.setupUi(this);
    ui.txt_qrData->hide();
    this->showMaximized(); // Tells Qt to open in full screen

    // -*- Initializing the camera -*-
    cap.open(0, cv::CAP_DSHOW); // Defualt camera is held a the first index 0
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); // setting resolution

    // Setup the timer to act as your "infinite loop"
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateFrame);
    timer->start(33); // roughly 30 FPS
}

void MainWindow::updateFrame() 
{
    cv::Mat frame; // creating the frame variable that is used to store the current video frame
    cap >> frame; // using the cap object to store its output into frame

    if (!frame.empty()) 
    {
        // Checks to see if camera is working
        std::string data = decoder.detectAndDecode(frame);

        if (!data.empty()) 
        {
            currentUrl = data;

            ui.txt_qrData->setText(QString("<a href='%1' style='color: #0000FF; text-decoration: underline;'>%1</a>").arg(QString::fromStdString(data)));
            ui.txt_qrData->show(); // Show it when data is found
        }
    }

    // -*- Convert MAT (OpenCV) To QIMAGE (QT) -*-
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage qimg((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    // Prepare the Canvas
    QPixmap roundedPixmap(ui.lbl_video->size());
    roundedPixmap.fill(Qt::transparent); // Makes the corners see-through

    // Setup the Painter
    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Scale the image first to find its final size
    QPixmap scaledVideo = QPixmap::fromImage(qimg.copy()).scaled(ui.lbl_video->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Calculate offsets to center the video in the label
    int x = (ui.lbl_video->width() - scaledVideo.width()) / 2;
    int y = (ui.lbl_video->height() - scaledVideo.height()) / 2;

    // Create the rounded clipping path at the centered position
    QPainterPath path;
    path.addRoundedRect(QRectF(x, y, scaledVideo.width(), scaledVideo.height()), 30, 30);
    painter.setClipPath(path);

    // Draw the video at the calculated center
    painter.drawPixmap(x, y, scaledVideo);
    painter.end();


    // Finally, show the rounded result
    ui.lbl_video->setPixmap(roundedPixmap);
}

void MainWindow::on_btn_openUrl_clicked() // -*- Open Button Actions -*-
{
    if (!currentUrl.empty()) // if a url is scanned
    {
        ShellExecuteA(NULL, "open", currentUrl.c_str(), NULL, NULL, SW_SHOWNORMAL); // This opens the URL in the default browser
    }

    else // if no url is scanned
    {
        QMessageBox::warning(this, "Error", "No URL detected!");
    }
}