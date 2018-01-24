/*texteditdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to add or edit custom text item on the operating panel
 * called by myScene.cpp
 */



#include "texteditdialog.h"
#include "ui_texteditdialog.h"
#include "mainwindow.h"

extern MainWindow *theMainwindow;

textedit::textedit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::textedit)
{
    ui->setupUi(this);
}

textedit::textedit(QWidget *parent, SimpleTextItem * textitem):
    QDialog(parent),ui(new Ui::textedit)
{
    ui->setupUi(this);
    QStringList colorNames;
    colorNames.append("red");
    colorNames.append("black");
    colorNames.append("blue");
    colorNames.append("yellow");
    colorNames.append("green");
    colorNames.append("gray");
    colorNames.append("orange");
    colorNames.append("purple");

    for (int i = 0; i < colorNames.size(); ++i)
    {
        QColor color(colorNames[i]);
        ui->comboBox_color->insertItem(i, colorNames[i]);
        ui->comboBox_color->setItemData(i, color, Qt::DecorationRole);

    }
    ui->comboBox_color->setCurrentIndex(ui->comboBox_color->findData(textitem->brush().color(), Qt::DecorationRole));
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setWindowTitle("Add customized text");
    this->text=textitem;
    ui->textEdit->setPlainText(textitem->text());
    ui->spinBox_size->setValue(textitem->font().pointSize());
    ui->checkBox_bold->setChecked(textitem->font().bold());
    ui->checkBox_italic->setChecked(textitem->font().italic());
    ui->checkBox_underline->setChecked(textitem->font().underline());
}


textedit::~textedit()
{
    delete ui;
}

void textedit::on_buttonBox_accepted()
{

    QFont font;
    font.setBold(ui->checkBox_bold->isChecked());
    font.setItalic(ui->checkBox_italic->isChecked());
    font.setUnderline(ui->checkBox_underline->isChecked());
    font.setPointSize(ui->spinBox_size->text().toInt());
    text->setText(ui->textEdit->toPlainText());
    text->setFont(font);
    text->setBrush(QBrush(QColor(ui->comboBox_color->currentText())));

}

bool textedit::event(QEvent *e)
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

