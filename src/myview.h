#ifndef MYVIEW_H
#define MYVIEW_H

#include <QWidget>
#include <QGraphicsView>


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
