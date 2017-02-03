#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>

namespace Ui {
class startDialog;
}

class startDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit startDialog(QWidget *parent = 0);
    ~startDialog();
    
private slots:
    void closeEvent(QCloseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void on_nextButton_clicked();

    void on_openButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_templateList_doubleClicked(const QModelIndex &index);

    void on_recentList_doubleClicked(const QModelIndex &index);

    void on_importButton_clicked();

private:
    Ui::startDialog *ui;

    void focusInEvent(QFocusEvent *e);
    bool event(QEvent *e);
    bool loadRecentFiles();

};

#endif // STARTDIALOG_H
