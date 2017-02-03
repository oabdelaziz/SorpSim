#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "node.h"
#include "link.h"
#include <QGraphicsView>
#include "ui_mainwindow.h"
#include <QGraphicsEllipseItem>
#include <QtXml>
#include "myscene.h"
#include "unit.h"
#include "myview.h"
#include "tableselectparadialog.h"
#include "calculate.h"
#include <QDockWidget>
#include <QCloseEvent>
#include <QToolBar>
#include <QDialog>
#include <QPrinter>

class Node;
class Link;

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QDialog*currentDialog;

    bool setTPMenu();
    bool setRecentFiles();
    bool saveRecentFile(QString fileDir);

    void disableResult();
    bool resultsShown;
    void resultShow();
    void switchToSelect();

private slots:

    void on_actionNew_triggered();

    void on_actionComponent_triggered();

    void on_actionExit_triggered();

    void on_actionDelete_triggered();

    void on_actionProperties_triggered();

    void on_actionRun_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionAbout_triggered();

    void evokeAbout();

    void debugging();

    void printPreview(QPrinter*printer);

    void saveFile(QString fileName, bool overwrite);

    void on_actionSave_As_triggered();

    void on_actionText_triggered();

    void on_actionNew_Parametric_Table_triggered();

    void on_actionPrint_triggered();

    void on_actionContent_And_Index_triggered();

    void on_actionChoose_Results_triggered();

    void chooseResults();

    void on_actionShow_Results_triggered();

    void on_actionTable_Window_triggered();

    void on_actionNew_Parametric_Plot_triggered();

    void on_actionNew_Property_Plot_triggered();

    void on_actionLine_triggered();

    void on_actionMaster_Control_Panel_triggered();

    void on_actionImport_out_File_triggered();

    void on_actionSelect_triggered();

    void on_actionPan_triggered();

    void on_actionZoom_To_Fit_triggered();

    void on_actionSystem_Settings_triggered();

    void closeEvent(QCloseEvent *event);

    void on_actionExport_to_File_triggered();

    void keyPressEvent(QKeyEvent * event);

    void on_actionAdditional_equations_triggered();

    void on_actionRotate_Clockwise_triggered();

    void on_actionFlip_Horizontally_triggered();

    void on_actionFlip_Vertically_triggered();

    void on_actionPlot_Window_triggered();

    void openTableWindow();

    void openPlotWindow();    

    void openRecentFile();

    void on_actionCalculation_Details_triggered();

    void loadExampleCase();

    void on_actionResults_Table_triggered();

    void on_actionGuess_Value_triggered();

private:
    Ui::MainWindow *ui;

    void defaultTheSystem();
    void clearResultSelection();
    int hasCopcap();
    void setZValue(int z);
    void setupNode(Node *node);
    void deleteunit(unit * delunit);
    void deletelink(Link * dellink);
    void zoomToFit();
    void startWindow();
    void newCase();
    bool openCase();
    int askToSave();
    bool loadCase(QString name);
    bool preprocessOutFile(QString fileName);
    bool loadOutFile();
    void manageGroups();
    void createGroupFromIfix();
    bool noChangeMade();
    QMap<QString,int> hasTPData(bool lookForTable);
    calculate * mycal;
    myScene *scene;
    myView *view;
    QAction* openTWindow;
    QAction* openPWindow;

    void resizeEvent(QResizeEvent *e);



};


#endif // MAINWINDOW_H
