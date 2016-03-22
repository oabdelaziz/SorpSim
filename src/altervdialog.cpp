/*altervdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the values of cells in an existing parametric table
 * collects the user input at the dialog, change the global boolean and numeric variables accordingly
 * the change in table and XML file is enforced in the tableDialog.cpp
 * called by tabledialog.cpp
 */


#include "altervdialog.h"
#include "ui_altervdialog.h"
#include "tabledialog.h"
#include <QMessageBox>
#include <QDoubleValidator>
#include <QDebug>
#include <QLayout>
#include <QValidator>
#include <QDoubleValidator>
#include "mainwindow.h"

extern bool alvIsEnter;
extern int alvFirstRow;
extern int alvLastRow;
extern double alvFirstValue;
extern double alvLastValue;
extern bool alvAccepted;
extern int alvMethod;
extern int alvCol;
extern int alvRowCount;

extern tableDialog*theTablewindow;
extern MainWindow*theMainwindow;

//THE LASTVALUE LAYOUT IS: LINEAR, INCREMENTAL, MULTIPLIER, LOG

altervDialog::altervDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::altervDialog)
{
    ui->setupUi(this);
    ui->enterValueGB->setDisabled(true);
    ui->firstRowSB->setValue(1);
    ui->lastRowSB->setValue(alvRowCount);
    ui->enterValueCB->setChecked(true);
    ui->firstValueLE->setText(QString::number(0));
    ui->lastValueLE->setText(QString::number(0));

    QRegExp regExp("[-.0-9]+$");
    QRegExpValidator *regExpValidator = new QRegExpValidator(regExp,this);
    ui->firstValueLE->setValidator(regExpValidator);
    ui->lastValueLE->setValidator(regExpValidator);

    setWindowFlags(Qt::Tool);
    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

altervDialog::~altervDialog()
{
    delete ui;
}

void altervDialog::setTableName(QString name)
{
    ui->tableName->setText(name);
}

void altervDialog::setInputs(QStringList inputs)
{
    ui->inputCB->insertItems(0,inputs);
}

void altervDialog::on_okButton_clicked()
{
    if((ui->firstRowSB->value()<ui->lastRowSB->value()&&ui->firstRowSB->value()>0)&&(ui->clearValueCB->isChecked()||ui->enterValueCB->isChecked())
            &&ui->firstValueLE->hasAcceptableInput()&&ui->lastValueLE->hasAcceptableInput())
        {
            alvFirstRow = ui->firstRowSB->value()-1;
            alvLastRow = ui->lastRowSB->value()-1;
            if(ui->enterValueCB->isChecked())
            {
                alvFirstValue = ui->firstValueLE->text().toDouble();
                alvLastValue = ui->lastValueLE->text().toDouble();
                switch(ui->lastCB->currentIndex())
                {
                case(0):
                {
                    alvMethod = 0;//LINEAR
                    break;
                }
                case(1):
                {
                    alvMethod = 1;//INCREMENTAL
                    break;
                }
                case(2):
                {
                    alvMethod = 2;//MULTIPLIER
                    break;
                }
                case(3):
                {
                    alvMethod = 3;//LOG
                    break;
                }
                }
                alvIsEnter = true;
            }
            else
                alvIsEnter = false;
            alvCol = ui->inputCB->currentIndex();

            accept();
            alvAccepted = true;
        }
        else// CAN ADD DETAILED DIRECTION TO WHERE THE PROBLEM IS
        {
            QMessageBox * alvBox = new QMessageBox;
            alvBox->setWindowTitle("Warning!");
            alvBox->setText("Please check your settings!");
            alvBox->exec();
        }
}

void altervDialog::on_cancelButton_clicked()
{
    reject();
    alvAccepted = false;
}

void altervDialog::on_enterValueCB_toggled(bool checked)
{
    if(!checked)
        ui->enterValueGB->setDisabled(true);
    else
        ui->enterValueGB->setDisabled(false);
}

void altervDialog::on_inputCB_currentIndexChanged(int index)
{
    double firstValue, lastValue;
    firstValue = ranges.at(index).split(",").first().toDouble();
    lastValue = ranges.at(index).split(",").last().toDouble();
    ui->firstValueLE->setText(QString::number(firstValue));
    ui->lastValueLE->setText(QString::number(lastValue));
}

bool altervDialog::event(QEvent *e)
{
    if(e->type()==QEvent::ActivationChange)
    {
        if(qApp->activeWindow()==this)
        {
            theMainwindow->show();
            theMainwindow->raise();
            theTablewindow->raise();
            this->raise();
            this->setFocus();
        }
    }
    return QDialog::event(e);
}
