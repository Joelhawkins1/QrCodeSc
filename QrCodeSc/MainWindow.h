#pragma once // preprocessor directive used in header files to ensure they are only processed one time per compilation unit
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h" 
#include <opencv2/opencv.hpp>
#include <QTimer>

namespace Ui { class QTMainWindow; } // new line to fix button

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private slots: 
    void updateFrame(); 
    void on_btn_openUrl_clicked(); // slot used for the open button.

private: // -*- objects and variables used across program -*-

    Ui::QTMainWindow ui; // MainWindow.ui Window

    cv::VideoCapture cap; // Video Capture object used by Opencv
    cv::QRCodeDetector decoder; // Detect and Decode object used by opencv

    QTimer* timer; // Timer object used as a replacement for a loop used by QT

    std::string currentUrl; // Stores the URL found by the camera


};