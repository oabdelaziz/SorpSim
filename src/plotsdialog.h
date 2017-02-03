#ifndef PLOTSDIALOG_H
#define PLOTSDIALOG_H

#include <QDialog>
#include "plotproperty.h"
#include <QTabWidget>
#include <QStatusBar>
#include <qwt_plot_marker.h>
#include <QToolButton>

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class Plot;
class QPolygon;

namespace Ui {
class plotsDialog;
}

class plotsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit plotsDialog(QString startPlot="", bool fromTable=false, QWidget *parent = 0);
    ~plotsDialog();
    
private slots:

    void mousePressEvent(QMouseEvent *event);

    void showContextMenu(const QPoint &pos);

    void refreshThePlot();

    void setupPlots(bool init=true);

    bool loadXml(bool init = true);

    void moved(const QPoint & pos);

    void selected( const QPolygon & );

    void overlay();

    void edit();

    void print();

    void exportDocument();

    void enableZoomMode(bool on);

    void showInfo( QString text = QString::null );

    void deleteCurrentPlot();

    void resetZoomer(int i);

    void on_exportMenuButton_triggered(QAction *arg1);

    void on_dataSelectButton_clicked();

    void on_copyButton_clicked();

    void closeEvent(QCloseEvent *);

    bool saveChanges();

    void keyPressEvent(QKeyEvent *e);

    void on_editButton_clicked();

    void on_zoomButton_toggled(bool checked);

private:
    Ui::plotsDialog *ui;
    QTabWidget* tabs;
    QwtPlotZoomer *d_zoomer[2];
    QwtPlotPicker *d_picker;
    QwtPlotPanner *d_panner;
    QStatusBar *statusBar;
    QToolButton * exportButton;
    QString startPName;
    bool isFromTable;
    QMenu* contextMenu;

    void savePlotSettings();
};

#endif // PLOTSDIALOG_H
