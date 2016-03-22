#ifndef OVERLAYSETTING_H
#define OVERLAYSETTING_H

#include <QDialog>
#include <plotproperty.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <cmath>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <pixmap.h>
#include <unitconvert.h>
#include <qwt_scale_draw.h>
#include <qwt_picker_machine.h>
#include <qwt_picker.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include "plotsdialog.h"
namespace Ui {
class overlaysetting;
}




class overlaysetting : public QDialog
{
    Q_OBJECT

public:
    ~overlaysetting();
    overlaysetting(Plot * d_plot, QWidget *parent=NULL);
    void displaylist();

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_pushButton_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_add_input_clicked();

    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject * object, QEvent * event);

    void updateXml();

    void on_addLoopButton_clicked();

    bool curveNameUsed(QString name);

    void keyPressEvent(QKeyEvent *e);

private:
    Ui::overlaysetting *ui;
    Plot *overlay_plot;
    QwtPlotCurve * newCurve;
    QList<QList<int> > myLoopList;

    QString plotUnit;

    void drawPlot();
    void updateLoopList();
    void debugg();
};

class axis_type
{
public:
    enum type{T,P,c,h};
};




#endif // OVERLAYSETTING_H
