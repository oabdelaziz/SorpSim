/*curvesettingdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit the properties of an existing plot and curves in that plot
 * access the Plot object and make changes to the curves instantly
 * called by plotsdialog.cpp
 */


#include "curvesettingdialog.h"
#include <qcombobox.h>
#include <QComboBox>
#include "ui_curvesettingdialog.h"
#include <QLayout>
#include <QDebug>
#include "mainwindow.h"
#include "dataComm.h"
#include <qwt_symbol.h>
#include "editpropertycurvedialog.h"

extern globalparameter globalpara;
extern MainWindow* theMainwindow;

curvesetting::curvesetting(QList<QwtPlotCurve *> * curvelist, Plot *plot, QWidget *parent) :
    QDialog(parent), ui(new Ui::curvesetting)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool);
    setWindowTitle("Plot options");
    QLayout *mainLayout = layout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(mainLayout);


    QRegExp regExp2("[0-9]+$");
    QRegExpValidator *pRegExpValidator2 = new QRegExpValidator(regExp2,this);
    ui->lineEdit_legendcol->setValidator(pRegExpValidator2);
    ui->lineEdit_legendsize->setValidator(pRegExpValidator2);

//value set
    curvelistset=curvelist;
    set_plot=plot;
    ui->lineEdit_Title->setText(set_plot->title().text());
    QString col=QString::number(set_plot->internalLegend->maxColumns());
    QString size=QString::number(set_plot->internalLegend->font().pointSize());

//initialize legend
    if (set_plot->externalLegend!=NULL)
    {
        ui->checkBox_legend->setChecked(true);
        ui->radio_Legend1->setChecked(true);
        ui->positionBox->setCurrentIndex(set_plot->plotLayout()->legendPosition());
        on_checkBox_legend_clicked();

    }
    else if(set_plot->internalLegend->isVisible())
    {
        ui->checkBox_legend->setChecked(true);
        ui->radio_Legend2->setChecked(true);

        if (set_plot->internalLegend->alignment()==(Qt::AlignBottom|Qt::AlignLeft))
        {
            ui->hAlignmentBox->setCurrentIndex(0);
            ui->vAlignmentBox->setCurrentIndex(2);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignVCenter|Qt::AlignLeft))
        {
            ui->hAlignmentBox->setCurrentIndex(0);
            ui->vAlignmentBox->setCurrentIndex(1);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignTop | Qt::AlignLeft))
        {
            ui->hAlignmentBox->setCurrentIndex(0);
            ui->vAlignmentBox->setCurrentIndex(0);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignBottom | Qt::AlignHCenter))
        {
            ui->hAlignmentBox->setCurrentIndex(1);
            ui->vAlignmentBox->setCurrentIndex(2);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignVCenter | Qt::AlignHCenter))
        {
            ui->hAlignmentBox->setCurrentIndex(1);
            ui->vAlignmentBox->setCurrentIndex(1);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignTop | Qt::AlignHCenter))
        {
            ui->hAlignmentBox->setCurrentIndex(1);
            ui->vAlignmentBox->setCurrentIndex(0);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignBottom | Qt::AlignRight))
        {

            ui->hAlignmentBox->setCurrentIndex(2);

            ui->vAlignmentBox->setCurrentIndex(2);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignVCenter | Qt::AlignRight))
        {
            ui->hAlignmentBox->setCurrentIndex(2);
            ui->vAlignmentBox->setCurrentIndex(1);
        }
        else if (set_plot->internalLegend->alignment()==(Qt::AlignTop | Qt::AlignRight))
        {
            ui->hAlignmentBox->setCurrentIndex(2);
            ui->vAlignmentBox->setCurrentIndex(0);
        }

        on_checkBox_legend_clicked();
    }
    else
    {
        ui->positionBox->setCurrentIndex(1);
        ui->checkBox_legend->setChecked(false);
        on_checkBox_legend_clicked();
        set_plot->internalLegend->setVisible(false);
    }
// color combo
    QStringList colorNames = QColor::colorNames();
    for (int i = 0; i < colorNames.size(); ++i)
    {
        QColor color(colorNames[i]);
        ui->comboBox->insertItem(i, colorNames[i]);
        ui->comboBox->setItemData(i, color, Qt::DecorationRole);
        ui->comboBox_backcolor->insertItem(i, colorNames[i]);
        ui->comboBox_backcolor->setItemData(i, color, Qt::DecorationRole);
        ui->comboBox_color_x->insertItem(i, colorNames[i]);
        ui->comboBox_color_x->setItemData(i, color, Qt::DecorationRole);
        ui->comboBox_color_xmin->insertItem(i, colorNames[i]);
        ui->comboBox_color_xmin->setItemData(i, color, Qt::DecorationRole);
    }

    setLists();
//initialization

    ui->lineEdit_legendcol->setText(col);


    ui->lineEdit_legendsize->setText(size);
    ui->lineEdit_Title->setText(set_plot->title().text());
    ui->lineEdit_X_axis->setText(set_plot->axisTitle(QwtPlot::xBottom).text());
    ui->lineEdit_Y_axis->setText(set_plot->axisTitle(QwtPlot::yLeft).text());

    if (set_plot->grid->xMinEnabled()) ui->checkBox_enablexm->setChecked(true);
    if (set_plot->grid->yMinEnabled()) ui->checkBox_enableym->setChecked(true);
    if (set_plot->grid->xEnabled()) ui->checkBox_enablex->setChecked(true);
    if (set_plot->grid->xEnabled()) ui->checkBox_enabley->setChecked(true);

    if (set_plot->grid->xEnabled()||set_plot->grid->yEnabled())
    {
        ui->spinBox_major->setValue(set_plot->grid->majorPen().width());
        ui->comboBox_color_x->setCurrentIndex(ui->comboBox_color_x->findData(set_plot->grid->majorPen().color(), Qt::DecorationRole));
        ui->comboBox_line_major->setCurrentIndex(set_plot->grid->majorPen().style());
    }
    if (set_plot->grid->xMinEnabled()||set_plot->grid->yMinEnabled())
    {
        ui->spinBox_minor->setValue(set_plot->grid->minorPen().width());
        ui->comboBox_color_xmin->setCurrentIndex(ui->comboBox_color_xmin->findData(set_plot->grid->minorPen().color(), Qt::DecorationRole));
        ui->comboBox_line_minor->setCurrentIndex(set_plot->grid->minorPen().style());
    }

    ui->comboBox_backcolor->setCurrentIndex(ui->comboBox_backcolor->findData(set_plot->canvasBackground().color(), Qt::DecorationRole));


    connect(ui->comboBox_backcolor,SIGNAL(currentIndexChanged(int)),SLOT(setupmargin()));
    connect(ui->spinBox_major,SIGNAL(valueChanged(int)),SLOT(setupgrid()));
    connect(ui->spinBox_minor,SIGNAL(valueChanged(int)),SLOT(setupgrid()));
    connect(ui->checkBox_enablex,SIGNAL(clicked()),SLOT(setupgrid()));
    connect(ui->checkBox_enablexm,SIGNAL(clicked()),SLOT(setupgrid()));
    connect(ui->checkBox_enabley,SIGNAL(clicked()),SLOT(setupgrid()));
    connect(ui->checkBox_enableym,SIGNAL(clicked()),SLOT(setupgrid()));
    connect(ui->comboBox_line_major,SIGNAL(currentIndexChanged(int)),SLOT(setupgrid()));
    connect(ui->comboBox_line_minor,SIGNAL(currentIndexChanged(int)),SLOT(setupgrid()));
    connect(ui->comboBox_color_x,SIGNAL(currentIndexChanged(int)),SLOT(setupgrid()));
    connect(ui->comboBox_color_xmin,SIGNAL(currentIndexChanged(int)),SLOT(setupgrid()));

    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->label_4,SLOT(setVisible(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->positionBox,SLOT(setVisible(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->label_5,SLOT(setHidden(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->label_6,SLOT(setHidden(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->label_7,SLOT(setHidden(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->label_8,SLOT(setHidden(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->hAlignmentBox,SLOT(setHidden(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->vAlignmentBox,SLOT(setHidden(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->lineEdit_legendcol,SLOT(setHidden(bool)));
    connect(ui->radio_Legend1,SIGNAL(toggled(bool)),ui->lineEdit_legendsize,SLOT(setHidden(bool)));    

    // Triggers to update legend
    // position combobox
    connect(ui->positionBox,         SIGNAL(currentIndexChanged(int)),     SLOT(setuplegend()));
    // horizontal alignment combobox
    connect(ui->hAlignmentBox,       SIGNAL(currentIndexChanged(int)),     SLOT(setuplegend()));
    // vertical alignment combobox
    connect(ui->vAlignmentBox,       SIGNAL(currentIndexChanged(int)),     SLOT(setuplegend()));
    // internal legend column line
    connect(ui->lineEdit_legendcol,  SIGNAL(textChanged(const QString &)), SLOT(setuplegend()));
    // internal legend size line
    connect(ui->lineEdit_legendsize, SIGNAL(textChanged(const QString &)), SLOT(setuplegend()));
}

curvesetting::~curvesetting()
{
    delete ui;
}

void curvesetting::on_pushButton_2_clicked()//accept button
{
    accept();
}

void curvesetting::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
}

void curvesetting::on_lineEdit_linetitle_textChanged(const QString &arg1)//set curve title
{
    if(ui->listWidget->count()>0&&ui->listWidget->currentRow()!=-1)
    {
        int bgLineCount = ui->bgList->count();
        int listIndex = ui->listWidget->currentRow()+bgLineCount;
        QString oldCurveName = curvelistset->at(listIndex)->title().text();
        QString curveName = arg1;
        if(curveName.count()==0)
            curveName = "curve_"+QString::number(ui->listWidget->currentRow());
        if(curveName.at(0).isDigit())
            curveName = "curve_"+curveName;
        curvelistset->at(listIndex)->setTitle(curveName);
        ui->lineEdit_linetitle->setText(curveName);
    }
}

void curvesetting::on_comboBox_currentIndexChanged(QString text)//line color combobox
{    
    int bgLineCount = ui->bgList->count();
    QColor color(text);
    if (ui->listWidget->currentRow()<0) return;
    curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->setPen(color,curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->pen().width(),curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->pen().style());
    set_plot->replot();
}

void curvesetting::on_listWidget_currentRowChanged(int currentRow)//load current line status
{
int rowsInListWidget = ui->listWidget->count();
// Double check that currentRow hasn't just been deleted before accessing it.
#ifdef QT_DEBUG
    qDebug() << "curvesetting::on_listWidget_currentRowChanged debug info:";
    qDebug() << "Count of rows in the listWidget:" << rowsInListWidget;
    qDebug() << "Which currentRow was passed:" << currentRow;
    if (currentRow < rowsInListWidget)
        qDebug() << "The currentRow is a valid index into the list.";
    else
    {
        qDebug() << "Clearly not enough lists! Giving up gracefully.";
        qDebug() << "Just to follow up, asking for the currentRow now gives" << ui->listWidget->currentRow();
        return;
    }
#endif
    if(ui->listWidget->count()>0&&currentRow!=-1)
    {
        ui->curveActionBox->setEnabled(true);
        int bgLineCount = ui->bgList->count();
        if (!(currentRow<ui->listWidget->count())) return;
        ui->comboBox->setCurrentIndex(ui->comboBox->findData(curvelistset->at(currentRow+bgLineCount)->pen().color(), Qt::DecorationRole));
        int width=curvelistset->at(currentRow+bgLineCount)->pen().width();
        if (width==0) ui->lineWidthSB->setValue(1);
        ui->lineWidthSB->setValue(curvelistset->at(currentRow+bgLineCount)->pen().width());
        ui->comboBox_2->setCurrentIndex(curvelistset->at(currentRow+bgLineCount)->pen().style());


        QString curveName = curvelistset->at(currentRow+bgLineCount)->title().text();

        ui->lineEdit_linetitle->setText(curveName);
    }
    else
        ui->curveActionBox->setDisabled(true);
}

void curvesetting::on_comboBox_2_currentIndexChanged(int index)//line style combobox
{
    int bgLineCount = ui->bgList->count();
    curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->setPen(curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->pen().color(),curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->pen().width(),Qt::PenStyle(index));
    set_plot->replot();
}

void curvesetting::on_checkBox_legend_clicked()//enable/disable legend checkbox
{
    if (ui->checkBox_legend->isChecked()==false)
    {
        set_plot->externalLegend = NULL;
        set_plot->insertLegend(NULL);
        set_plot->internalLegend->setVisible(false);
        ui->groupBox_2->setEnabled(false);
        set_plot->replot();
        set_plot->setContentsMargins(5,5,5,5);
    }
    else
    {
        ui->groupBox_2->setEnabled(true);
        if (!set_plot->internalLegend->isVisible())ui->radio_Legend1->setChecked(true);
        on_radio_Legend1_clicked();
        on_radio_Legend2_clicked();
    }
}

void curvesetting::setuplegend()//legend position settings
{
    if (ui->radio_Legend1->isChecked())//internal/external legend
    {
        set_plot->externalLegend=new QwtLegend();
        set_plot->insertLegend(set_plot->externalLegend,QwtPlot::LegendPosition(ui->positionBox->currentIndex()));//position of external legend
    }
    else
    {
        set_plot->externalLegend=NULL;
        set_plot->insertLegend(NULL);

        //alignment setting
        int align = 0;

        int hIndex = ui->hAlignmentBox->currentIndex();//horizontal align
        if ( hIndex == 0 )
            align |= Qt::AlignLeft;
        else if ( hIndex == 2 )
            align |= Qt::AlignRight;
        else
            align |= Qt::AlignHCenter;

        int vIndex = ui->vAlignmentBox->currentIndex();//vertical align
        if ( vIndex == 0 )
            align |= Qt::AlignTop;
        else if ( vIndex == 2 )
            align |= Qt::AlignBottom;
        else
            align |= Qt::AlignVCenter;
        set_plot->internalLegend->setAlignment( Qt::Alignment(align ) );

        // other setting
        set_plot->internalLegend->setMaxColumns(ui->lineEdit_legendcol->text().toInt());
        QFont font = set_plot->internalLegend->font();
        font.setPointSize( fmax(ui->lineEdit_legendsize->text().toInt(),0));
        set_plot->internalLegend->setFont( font );
    }
    set_plot->replot();

}

void curvesetting::on_radio_Legend1_clicked()//external legend button
{
    if (!ui->radio_Legend1->isChecked()) return;
    set_plot->internalLegend->setVisible(false);
    set_plot->externalLegend=new QwtLegend();
    set_plot->insertLegend(set_plot->externalLegend,QwtPlot::LegendPosition(ui->positionBox->currentIndex()));
    set_plot->replot();
}

void curvesetting::on_radio_Legend2_clicked()//internal legend button
{
    if(!ui->radio_Legend2->isChecked()) return;
    set_plot->externalLegend=NULL;
    set_plot->insertLegend(NULL);
    set_plot->internalLegend->setVisible(true);
    set_plot->replot();
    set_plot->setContentsMargins(5,5,5,5);
}

void curvesetting::setupmargin()//set background color
{
    QColor color(ui->comboBox_backcolor->currentText());
    set_plot->setCanvasBackground(color);
    set_plot->replot();
}

void curvesetting::on_lineEdit_Title_textChanged(const QString &arg1)//set plot title
{
    set_plot->setTitle(arg1);
}

void curvesetting::setupgrid()//grid (major/minor) and color
{
    set_plot->grid->enableX(ui->checkBox_enablex->isChecked());
    set_plot->grid->enableY(ui->checkBox_enabley->isChecked());
    set_plot->grid->enableXMin(ui->checkBox_enablexm->isChecked());
    set_plot->grid->enableYMin(ui->checkBox_enableym->isChecked());
    QColor color(ui->comboBox_color_x->currentText());
    set_plot->grid->setMajorPen(color,ui->spinBox_major->value(),Qt::PenStyle(ui->comboBox_line_major->currentIndex()));
    QColor color2(ui->comboBox_color_xmin->currentText());
    set_plot->grid->setMinorPen(color2,ui->spinBox_minor->value(),Qt::PenStyle(ui->comboBox_line_minor->currentIndex()));

    set_plot->replot();
}

void curvesetting::on_lineEdit_X_axis_textChanged(const QString &arg1)//x axis title
{
    set_plot->setAxisTitle(QwtPlot::xBottom,arg1);
}

void curvesetting::on_lineEdit_Y_axis_textChanged(const QString &arg1)//y axis title
{
    set_plot->setAxisTitle(QwtPlot::yLeft,arg1);
}

void curvesetting::curveToggled(QListWidgetItem *item)
{
    QString curveName = item->text();
    int curveIndex;
    if(curveName.contains("%"))
    {
        curveName.replace("% concentration","");
        curveName.prepend("baseConcentration_");
    }
    int bgLineCount = ui->bgList->count();
    QwtPlotCurve* thisCurve;
    bool found = false;
    for(int i = 0; i < curvelistset->count()-bgLineCount&&!found;i++)
    {
        if(curvelistset->at(i+bgLineCount)->title().text()==curveName)
        {
            thisCurve = curvelistset->at(i+bgLineCount);
            found = true;
            curveIndex = i;
        }

    }
    if(found)
    {
        if(item->checkState()==Qt::Checked)
        {
            thisCurve->attach(set_plot);
            thisCurve->setVisible(true);
            // BUG (fixed?): Sometimes get an index beyond size.
            if (set_plot->curveMarkers.size() > curveIndex)
            {
                foreach(QwtPlotMarker*marker,set_plot->curveMarkers.at(curveIndex))
                {
                    marker->attach(set_plot);
                    marker->setVisible(true);
                }
            }
        }
        else if(item->checkState()==Qt::Unchecked)
        {
            thisCurve->detach();
            thisCurve->setVisible(false);
            // BUG (fixed?): next line triggers an error when user unchecks all curves
            // Happens because curveMarkers may not exist for this curve.
            // See Plot::setupNewPropertyCurve (only called from plotsDialog::loadXml)
            if (set_plot->curveMarkers.size() > curveIndex)
            {
                foreach(QwtPlotMarker*marker,set_plot->curveMarkers.at(curveIndex))
                {
                    marker->detach();
                    marker->setVisible(false);
                }
            }
        }
        set_plot->replot();
    }
    else
        qDebug()<<"line not found";
}

void curvesetting::bgCurveToggled(QListWidgetItem *item)
{
    QString curveName = item->text();
    if(curveName.contains("%"))
    {
        curveName.replace("% concentration","");
        curveName.prepend("baseConcentration_");
    }
    QwtPlotCurve* thisCurve;
    int lineCount = ui->bgList->count();
    bool found = false;
    for(int i = 0; i < lineCount&&!found;i++)
    {
        if(curvelistset->at(i)->title().text()==curveName)
        {
            thisCurve = curvelistset->at(i);
            found = true;
        }
    }
    if(found)
    {
        if(item->checkState()==Qt::Checked)
        {
            thisCurve->attach(set_plot);
            thisCurve->setVisible(true);
            set_plot->replot();
        }
        else if(item->checkState()==Qt::Unchecked)
        {
            thisCurve->detach();
            set_plot->replot();
            thisCurve->setVisible(false);
        }

    }
    else
        qDebug()<<"line not found";

}

void curvesetting::on_editCurveButton_clicked()
{
    int bgLineCount = ui->bgList->count();
    editPropertyCurveDialog eDialog(set_plot,curvelistset,ui->listWidget->currentIndex().row(),bgLineCount,this);
    eDialog.exec();
}

void curvesetting::on_lineWidthSB_valueChanged(int arg1)//line width SpinBox
{
    int bgLineCount = ui->bgList->count();
    curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->setPen(curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->pen().color(),arg1,curvelistset->at(ui->listWidget->currentRow()+bgLineCount)->pen().style());
    set_plot->replot();
}

void curvesetting::on_deleteCurveButton_clicked()
{
    QString delCurveName;
    int bgLineCount = ui->bgList->count();
    delCurveName = set_plot->curvelist.at(bgLineCount+ui->listWidget->currentIndex().row())->title().text();
    int myRow = ui->listWidget->currentRow();
    int markerRows = set_plot->curveMarkers.count();
    // TODO: more permanent fix.
    // Take caution here, if curve is missing somehow.
    // Hint: curveMakers are only created for specialty plots (Duhring, Clapeyron)
    // See Plot::setupNewPropertyCurve
#ifdef QT_DEBUG
    qDebug() << "set_plot->curveMarkers debug info:";
    qDebug() << "Count of lists of curve markers:" << markerRows;
    qDebug() << "Looking for list number" << myRow;
#endif
    if (myRow < markerRows)
    {
        foreach (QwtPlotMarker * marker, set_plot->curveMarkers.at(myRow))
            marker->detach();
        set_plot->curveMarkers.removeAt(myRow);
    }

    set_plot->curvelist.at(bgLineCount+ui->listWidget->currentIndex().row())->detach();
    set_plot->curvelist.removeAt(bgLineCount+ui->listWidget->currentIndex().row());
    set_plot->replot();
    set_plot->curvePoints.removeAt(ui->listWidget->currentIndex().row());

    delete ui->listWidget->takeItem(ui->listWidget->currentRow());

#ifdef Q_OS_WIN32
    QFile file("plotTemp.xml");
#endif
#ifdef Q_OS_MAC
    QDir dir = qApp->applicationDirPath();
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    /*dir.cdUp();*/
    QString bundleDir(dir.absolutePath());
    QFile file(bundleDir+"/plotTemp.xml");
#endif
    QTextStream stream;
    stream.setDevice(&file);
    QDomDocument doc;
    QDomElement plotData, currentPlot;
    QDomNode thisCurve;

    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        globalpara.reportError("Fail to open case file for overlay setting.",this);
        return;
    }
    else
    {
        if(!doc.setContent(&file))
        {
            globalpara.reportError("Fail to load xml document for overlay setting.",this);
            file.close();
            return;
        }
        else
        {
            plotData = doc.elementsByTagName("plotData").at(0).toElement();

            // TODO: <plotData> children need valid XML tag names and title, eg
            // <plotData>
            //   <plot title="plot_1" plotType="property">
            //     <curveList/>
            //   </plot>
            // </plotData>
            currentPlot = plotData.elementsByTagName(set_plot->title().text().replace(" ","")).at(0).toElement();
            if(currentPlot.attribute("plotType")!="property")
            {
                qDebug()<<"wrong plot type";
                file.close();
                return;
            }
            else
            {
                QDomNodeList curveNodeList = currentPlot.elementsByTagName("curveList").at(0).childNodes();
                bool found = false;
                for (int i = 0; i < curveNodeList.count(); ++i)
                {
                    thisCurve = curveNodeList.at(i);
                    if (thisCurve.toElement().attribute("title") == delCurveName)
                    {
                        qDebug()<<"revoming curve"<<delCurveName;
                        currentPlot.removeChild(thisCurve);
                        found = true;
                    }
                }
                if (!found) {
                    qDebug() << "Failed to find in XML any <curve> with title" << delCurveName << ".";
                }
                file.resize(0);
                doc.save(stream,4);
                file.close();
                return;
            }
        }
    }
}

void curvesetting::setLists()
{
    ui->bgList->clear();
    ui->listWidget->clear();
    disconnect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(curveToggled(QListWidgetItem*)));
    disconnect(ui->bgList,SIGNAL(itemChanged(QListWidgetItem*)),this,SLOT(bgCurveToggled(QListWidgetItem*)));
    //background list and curve list widget
//    QwtPlotCurve* thisCurve;
    // TODO: find a better way to flag specialized plots
    if(curvelistset->count()>=28)//duhring
    {
        for(int i = 0; i < 28; i++)
        {
            QString curveName = curvelistset->at(i)->title().text();
            curveName.replace("[","");
            curveName.replace("]","");
            if(curveName.split("_").first()=="baseConcentration")
                curveName = curveName.split("_").last().append("% concentration");
            QListWidgetItem * item = new QListWidgetItem(curveName);
            ui->bgList->addItem(item);
            item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);

            QwtPlotCurve*thisCurve = curvelistset->at(i);
            if(thisCurve->isVisible())
                ui->bgList->item(i)->setCheckState(Qt::Checked);
            else
                ui->bgList->item(i)->setCheckState(Qt::Unchecked);
        }
        for(int i = 0; i < curvelistset->count()-28;i++)
        {
            QString curveName = curvelistset->at(i+28)->title().text();
            QListWidgetItem * item = new QListWidgetItem(curveName);
            ui->listWidget->addItem(item);
            item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);

            QwtPlotCurve*thisCurve = curvelistset->at(i+28);
            if(thisCurve->isVisible())
                ui->listWidget->item(i)->setCheckState(Qt::Checked);
            else
                ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    }
    else if(curvelistset->count()>=7)//Clapeyron
    {
        for(int i = 0; i < 7; i++)
        {
            QString curveName = curvelistset->at(i)->title().text();
            curveName.replace("[","");
            curveName.replace("]","");
            if(curveName.split("_").first()=="baseConcentration")
                curveName = curveName.split("_").last().append("% concentration");
            QListWidgetItem * item = new QListWidgetItem(curveName);
            ui->bgList->addItem(item);
            item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);

            QwtPlotCurve*thisCurve = curvelistset->at(i);
            if(thisCurve->isVisible())
                ui->bgList->item(i)->setCheckState(Qt::Checked);
            else
                ui->bgList->item(i)->setCheckState(Qt::Unchecked);
        }
        for(int i = 0; i < curvelistset->count()-7;i++)
        {
            QString curveName = curvelistset->at(i+7)->title().text();
            QListWidgetItem * item = new QListWidgetItem(curveName);
            ui->listWidget->addItem(item);
            item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);

            QwtPlotCurve*thisCurve = curvelistset->at(i+7);
            if(thisCurve->isVisible())
                ui->listWidget->item(i)->setCheckState(Qt::Checked);
            else
                ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
        }

    }
    else
    {
        ui->bgGroupBox->hide();
        ui->editCurveButton->hide();
        for(int i=0;i<curvelistset->count();i++)
        {
            QString curveName = curvelistset->at(i)->title().text();
            curveName.replace("[","");
            curveName.replace("]","");
            if(curveName.split("_").first()=="baseConcentration")
                curveName = curveName.split("_").last().append("% concentration");
            QListWidgetItem * item = new QListWidgetItem(curveName);
            ui->listWidget->addItem(item);
            item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
            item->setCheckState(Qt::Checked);

            QwtPlotCurve*thisCurve = curvelistset->at(i);
            if(thisCurve->isVisible())
                ui->listWidget->item(i)->setCheckState(Qt::Checked);
            else
                ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
        }
    }
    if(ui->listWidget->count()>0)
        ui->listWidget->setCurrentRow(0);
    else ui->curveActionBox->setDisabled(true);

    connect(ui->listWidget,SIGNAL(itemChanged(QListWidgetItem*)),SLOT(curveToggled(QListWidgetItem*)));
    connect(ui->bgList,SIGNAL(itemChanged(QListWidgetItem*)),SLOT(bgCurveToggled(QListWidgetItem*)));
}
