#include "MainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) 
{
    QApplication a(argc, argv);
    MainWindow w; // calls the MainWindow class
    w.show(); // shows the window made
    return a.exec();
}