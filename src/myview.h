/*! \file myview.h
    \brief Provides the class myView.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef MYVIEW_H
#define MYVIEW_H

#include <QWidget>
#include <QGraphicsView>

/// Custom class based on QGraphicsView to control the viewing of the operating panel
/// - mainly handles zoom (scaling) actions and panning actions
/// - called by masterdialog.cpp
class myView : public QGraphicsView
{

public:
    explicit myView(QWidget *parent = 0);

signals:

public slots:

    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    double myScale;
    void setScale();

private slots:
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
};

#endif // MYVIEW_H
