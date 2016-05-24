/*main.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * start a new mainwindow and leave all interactions to the mainwindow
 * once the mainwindow is closed, the application is closed as well
 * first subroutine to run at SorpSim's launch
 */





#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
