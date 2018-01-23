/*! \file mainwindow.h
    \brief draw/transform the components and store component data

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


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


/*!
The mainwindow is the major operating interface of SorpSim, the first window
to show when the program launches, and last when users close SorpSim.

The mainwindow class subclasses the QMainwindow class, with custom methods
and data members defined to handle actions triggered by button click or
mouse/keyboard operations.

The buttons of the menu bar are defined using Qt Creator and edited in the
corresponding mainwindow.ui, while the bottons on the tool bar are defined
in the mainwindow constructor "Mainwindow::Mainwindow (QWidget* parent){}".

The operating panel below the toolbar is consisted of a myView (subclass of
QGraphicsView, defined in myview.cpp/h) and a myScene (subclass of
QGraphicsScene, defined in myscene.cpp/h). The "myView" was added onto the
mainwindow in the constructor function, and the "myScene" is added onto the
"myView" after that. (refer to Qt documentations about View and Scene
operations)
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QDialog*currentDialog;

    /**
     * @brief setTPMenu Updates the menu actions available in the Table and Plot menus based on table and plot counts.
     *
     * If there are multiple plots, this will try to open the current case XML file.
     * This will return false if that I/O operation fails.
     * TODO: None of the client code uses the return value. Redefine without it?
     * Also, this seems to happen occasionally, and it should not! TODO: debug
     *
     * @return Whether the operation succeeded.
     */
    bool setTPMenu();
    bool setRecentFiles();
    bool saveRecentFile(QString fileDir);

    void disableResult();
    bool resultsShown;
    void resultShow();
    void switchToSelect();

    /*! \name Mouse selection modes

        When say a dialog wants to perform a mouse selection through the scene, but
        disable other actions (menus and toolbars), it can call setSceneMode.
        The actions are re-enabled after the dialog is destroyed.
        The dialog can sign up to be notified when user presses ESC.

        \todo This attribute is intended to replace sceneActionIndex. However,
        it is not yet fully implemented.
    */
    //! \{

    /// \brief SceneActionIndex: state of operation for mouse selection.
    ///
    /// Used by dialogs that interact with theScene (for selecting and creating items).
    /// MainWindow responds to any key press event of "ESC" and sets the mode back to 0.
    /// myScene responds to mouse click signals.
    enum class SceneActionIndex {
        Default,      ///< Default: double-click to open a property dialog
        DrawUnit,     ///< single-click to draw/add a new component
        DrawLink,     ///< double-click state points (two to be linked)
        TableSelect,  ///< double-click state point or component to select an input/output variable
        PlotSelect    ///< double-click state point to overlay on property plot
    };
private:
    SceneActionIndex theMode; ///< The current state of operation.
public:
    /// Setter for theMode.
    void setSceneMode(SceneActionIndex index, QObject *caller = 0);
    /// Equivalent to setSceneMode(SceneActionIndex::Default).
    void defaultMode();
    /// Getter for theMode.
    SceneActionIndex getSceneMode() const;
    /// Enables/disables the menubar and toolbar.
    ///
    /// \todo Make private
    void setActionsEnabled(bool enableActions);

signals:

    /// \brief Emitted when user presses ESC during mouse selection mode.
    void cancel_mouse_select_operation();

    //! \}

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

    /**
     * @brief defaultTheSystem clears the list of units, all display text, and resets global parameters.
     */
    void defaultTheSystem();

    /**
     * @brief clearResultSelection resets all toggles for variables to display in results mode.
     */
    void clearResultSelection();

    /**
     * @brief hasCopcap determines whether capacity and COP are defined.
     * @return 0 if capacity is not defined; 1 if capacity but COP is defined; or 2 if both are defined.
     */
    int hasCopcap();

    /**
     * @brief deleteunit clears all references to the unit, then deletes it.
     *
     * deleteunit() clears all references to the given unit, then deletes it.
     * This includes:
     * - Removing all the unit's nodes from global groups.
     * - Clearing references to and deleting all links to and from the units nodes.
     * - Removing the unit from the global list of units and renumbering unit indexes.
     * - Delete the unit (deallocate memory)
     *
     * @param delunit Pointer to the unit to delete.
     */
    void deleteunit(unit * delunit);

    /**
     * @brief deletelink Removes the link from its nodes and renumbers following nodes.
     *
     * TODO: Decide whether to deallocate the memory.
     * Right now this doesn't actually delete the memory allocated for dellink.
     * (Client code usage varies, which is bad.)
     *
     * @param dellink
     */
    void deletelink(Link * dellink);

    void zoomToFit();
    void startWindow();
    void newCase();

    /**
     * @brief openCase prompts user to select a file to load.
     * @return True if and only if the user did not cancel the operation and it was successful.
     */
    bool openCase();

    /** Opens a modal dialog box to ask if user wants to
     * save before the next requested operation.
     *
     * @returns 0 indicating user selected QMessageBox::Cancel;
     *   1 if QMessageBox::Ok (save and continue);
     *   2 if QMessageBox::No (skip saving and continue).
     */
    int askToSave();
    bool loadCase(QString name);
    bool preprocessOutFile(QString fileName);
    bool loadOutFile();
    void manageGroups();
    void createGroupFromIfix();
    bool noChangeMade();
    /**
     * @brief hasTPData checks whether how tables and plots belong to the current case.
     *
     * It opens the current case XML file and searches there for the data, not within memory.
     * The return is a map with at least one key, as requested (table if lookForTable, else plot).
     * If the file was correctly opened and parse, it should return both keys!
     * History
     * -- 2017-12-29. Fixed: failed to explicitly close file before return.
     *
     * @param lookForTable a toggle
     * @return A map counting tables and plots, like {'table': 1, 'plot': 2}.
     */
    QMap<QString,int> hasTPData(bool lookForTable);

    myScene *scene;
    myView *view;

    // TODO: move to ui?
    QAction* openTWindow;
    QAction* openPWindow;

    void resizeEvent(QResizeEvent *e);

    /**
     * @brief SSGetTempFileName
     * @return A QTemporaryFile that will be automatically removed when this is destroyed.
     */
    QFile * SSGetTempFileName();

};


#endif // MAINWINDOW_H
