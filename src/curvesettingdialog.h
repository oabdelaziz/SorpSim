#ifndef CURVESETTING_H
#define CURVESETTING_H

#include <QDialog>
#include <qwt_plot_dict.h>
#include <QComboBox>
#include <qcombobox.h>
#include <QList>
#include <qlist.h>
#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_layout.h>
#include <qwt.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include "plotproperty.h"
#include <QListWidgetItem>

namespace Ui {
class curvesetting;
}


class curvesetting : public QDialog
{
    Q_OBJECT

public:
    curvesetting(QList<QwtPlotCurve *>  * curvelist, Plot * plot,QWidget *parent=NULL);
    ~curvesetting();

private slots:
    void closeEvent(QCloseEvent *event);

    void on_pushButton_2_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_listWidget_currentRowChanged(int currentRow);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_checkBox_legend_clicked();

    void setuplegend();

    void on_radio_Legend1_clicked();

    void on_radio_Legend2_clicked();

    void on_positionBox_currentIndexChanged(int index);

    void on_hAlignmentBox_currentIndexChanged(int index);

    void on_vAlignmentBox_currentIndexChanged(int index);

    void on_lineEdit_legendcol_textChanged(const QString &arg1);
    
    void on_lineEdit_legendsize_textChanged(const QString &arg1);

    void setupmargin();
    
    void on_lineEdit_Title_textChanged(const QString &arg1);

    void on_lineEdit_linetitle_textChanged(const QString &arg1);

    void setupgrid();

    void on_lineEdit_X_axis_textChanged(const QString &arg1);

    void on_lineEdit_Y_axis_textChanged(const QString &arg1);

    void curveToggled(QListWidgetItem* item);

    void bgCurveToggled(QListWidgetItem*item);

    void on_editCurveButton_clicked();

    void on_lineWidthSB_valueChanged(int arg1);

    void on_deleteCurveButton_clicked();

private:

    void setLists();
    Ui::curvesetting *ui;
    QList<QwtPlotCurve*> * curvelistset;
    Plot *set_plot;

};

#endif // CURVESETTING_H
