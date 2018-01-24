/*! \file startdialog.cpp

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu
    \author Nicholas Fette (nfette)

    \copyright 2015, UT-Battelle, LLC
    \copyright 2017-2018, Nicholas Fette

*/


#include "startdialog.h"
#include "ui_startdialog.h"
#include <QCloseEvent>
#include <QLayout>
#include "dataComm.h"
#include "mainwindow.h"

extern globalparameter globalpara;
extern MainWindow*theMainwindow;

startDialog::startDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::startDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setWindowTitle(" ");
    ui->label->setText("Welcome to SorpSim!");
    ui->label->setFont(QFont("Helvetica",10,QFont::Bold));

    loadRecentFiles();

    QListWidgetItem *item;
    QLabel* label;
    foreach(QString fileName,globalpara.recentFileList)
    {
        item = new QListWidgetItem;
        label = new QLabel;
        QStringList dir = fileName.split("/");
        dir.removeLast();
        QString name = fileName.split("/").last();
        label->setText(name+"\n"+dir.join("/")+"\n");
        item->setSizeHint(QSize(0,50));
        ui->recentList->addItem(item);
        ui->recentList->setItemWidget(item,label);
    }
    if(ui->recentList->count()>0)
        ui->recentList->setCurrentRow(0);

    ui->openButton->hide();
    ui->importButton->hide();

    item = new QListWidgetItem("Blank project");
    ui->templateList->addItem(item);

    item = new QListWidgetItem("Single-effect LiBr-water chiller");
    ui->templateList->addItem(item);

    item = new QListWidgetItem("Double-effect LiBr-water chiller");
    ui->templateList->addItem(item);

    item = new QListWidgetItem("Double-condenser-coupled, triple-effect LiBr-water chiller");
    ui->templateList->addItem(item);

    item = new QListWidgetItem("Generator-absorber heat exchanger, water-ammonia heat pump");
    ui->templateList->addItem(item);

    item = new QListWidgetItem("Adiabatic liquid desiccant cycle");
    ui->templateList->addItem(item);

    item = new QListWidgetItem("Internally cooled/heated liquid desiccant cycle");
    ui->templateList->addItem(item);

    ui->templateList->setCurrentRow(0);

    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    setWindowModality(Qt::ApplicationModal);

}

startDialog::~startDialog()
{
    delete ui;
}

void startDialog::closeEvent(QCloseEvent *event)
{
    globalpara.startOption="close";
    accept();
}

void startDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape)
    {

    }
}

void startDialog::focusInEvent(QFocusEvent *e)
{
    QDialog::focusInEvent(e);
}

bool startDialog::event(QEvent *e)
{
    if(e->type()==QEvent::ActivationChange)
    {
        if(qApp->activeWindow()==this)
        {
            theMainwindow->show();
            theMainwindow->raise();
            this->raise();
            this->setFocus();
        }
    }
    return QDialog::event(e);
}

bool startDialog::loadRecentFiles()
{
#ifdef Q_OS_WIN32
    QString fileName(QDir(qApp->applicationDirPath()).absolutePath()+"/platforms/systemSetting.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    QString fileName(dir.absolutePath()+"/templates/systemSetting.xml");
#endif
    QFile ofile(fileName);
    QDomDocument doc;
    if(!ofile.open(QIODevice::ReadOnly|QIODevice::Text))
        return false;
    else
    {
        if(!doc.setContent(&ofile))
        {
            ofile.close();
            return false;
        }
    }

    QDomElement recentFiles = doc.elementsByTagName("recentFiles").at(0).toElement();
    int fileCount = recentFiles.childNodes().count();

    if(fileCount==0)
        return false;
    else
    {
        QStringList fileList;
        globalpara.recentFileList.clear();
        for(int i = 0; i < fileCount;i++)
        {
            QDomElement currentFile = recentFiles.childNodes().at(fileCount - 1 - i).toElement();
            fileList<<currentFile.attribute("fileDir");
            fileList.removeDuplicates();
        }
        foreach(QString fileName,fileList)
            globalpara.recentFileList.append(fileName);

        return true;
    }

}

void startDialog::on_nextButton_clicked()
{
    if(ui->tabWidget->currentIndex()==0)//newTab
    {
         QString newString = ui->templateList->currentItem()->text();
         if(newString == "Blank project")
         {
             globalpara.startOption="new";
             accept();
         }
         else
         {
             globalpara.startOption="template@@"+newString;
             accept();
         }
    }
    else//openTab
    {

        globalpara.startOption = "recent@@"+globalpara.recentFileList.at(ui->recentList->currentRow());
        accept();
    }
}

void startDialog::on_openButton_clicked()
{
    globalpara.startOption="browse";
    accept();
}

void startDialog::on_tabWidget_currentChanged(int index)
{
    if(index==0)//new
    {
        ui->openButton->hide();
        ui->nextButton->setEnabled(true);
        ui->importButton->hide();
    }
    else
    {
        ui->openButton->show();
        if(ui->recentList->count()==0)
            ui->nextButton->setDisabled(true);
        ui->importButton->show();
    }
}

void startDialog::on_templateList_doubleClicked(const QModelIndex &index)
{
    QString newString = ui->templateList->item(index.row())->text();
    if(newString == "Blank project")
    {
        globalpara.startOption="new";
        accept();
    }
    else
    {
        globalpara.startOption="template@@"+newString;
        accept();
    }
}

void startDialog::on_recentList_doubleClicked(const QModelIndex &index)
{
    globalpara.startOption = "recent@@"+globalpara.recentFileList.at(index.row());
    accept();
}

void startDialog::on_importButton_clicked()
{
    globalpara.startOption = "import";
    accept();
}
