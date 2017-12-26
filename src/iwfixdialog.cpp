/*iwfixdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit vapor fraction variable groups
 * called by vicheckdialog.cpp
 */





#include "iwfixdialog.h"
#include "ui_iwfixdialog.h"
#include <QListView>
#include <QStringList>
#include "mainwindow.h"
#include "unit.h"
#include "node.h"
#include "link.h"
#include <QStringListModel>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QModelIndex>
#include <QSet>
#include "dataComm.h"
#include "vicheckdialog.h"

extern unit * dummy;
extern int globalcount;
extern globalparameter globalpara;
extern MainWindow*theMainwindow;
extern VICheckDialog*theVIDialog;

QStringList groupListW;
QStringList listListW;
QStringListModel * listModelW;

iwfixDialog::iwfixDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::iwfixDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Vapor Fraction");
    ui->label->setText("Please define state points that have same vapor fraction.");

    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);    
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    globalpara.resetIfixes('w');

    QStringList cbListW;
    QStringList tempListW;
    unit * iterator = dummy;
    for(int i = 0; i<globalcount;i++)
    {
        iterator= iterator->next;

        for(int j = 0; j<iterator->usp;j++)
        {
            if(iterator->myNodes[j]->iwfix>0)
            {
                Node* theNode = iterator->myNodes[j];
                QSet<Node*>wList;
                globalpara.allSet.clear();
                theNode->searchAllSet("w");
                wList = globalpara.allSet;

                if((theNode->linked&&wList.count()==2&&theNode==theNode->myLinks.toList().first()->myToNode)
                        ||(!theNode->linked&&wList.count()==1)){
//single point
                    if(!cbListW.contains("W#"+QString::number(theNode->ndum))){
                    cbListW.append("W#"+QString::number(theNode->ndum));

                    }

                }
                else if(!(theNode->linked&&wList.count()==2&&theNode==theNode->myLinks.toList().first()->myFromNode)){
//in a stream
                    tempListW.clear();
                    QList<int> list;
                    for(int i = 0; i < wList.count(); i++){
                        Node * node = wList.toList().at(i);
                        if(!list.contains(node->ndum)){
                            list.append(node->ndum);
                        }
                    }
                    qSort(list.begin(),list.end());

                    for(int i = 0; i < list.count(); i++){
                        tempListW.append(QString::number(list.at(i)));
                    }


                    if(!cbListW.contains("W-Stream: "+tempListW.join(","))){
                        cbListW.append("W-Stream: "+tempListW.join(","));
                }

                }

            }
        }
    }

    ui->tCB1->addItems(cbListW);
    ui->tCB2->addItems(cbListW);

    updateList();



}

iwfixDialog::~iwfixDialog()
{
    delete ui;
}

void iwfixDialog::updateTempSets()
{
    globalpara.resetIfixes('w');
    wSets.clear();
    QSet<Node*> scanSet;
    for(int n = 0; n < globalpara.wGroup.count();n++){
        QSet<Node*> tempSet = globalpara.wGroup.at(n);
        QList<QSet<Node*> > newBigSet;

        for(int m = 0; m < tempSet.count(); m++){
            Node* node = tempSet.toList().at(m);
            bool isIn = false;
            foreach(QSet<Node*> smallSet, newBigSet){
                if(smallSet.contains(node)){
                    isIn = true;
                }
            }
            if(!isIn){
                globalpara.allSet.clear();
                node->searchAllSet("w");
                scanSet = globalpara.allSet;
                newBigSet.append(scanSet);
            }

        }
        wSets.append(newBigSet);
    }
}

void iwfixDialog::updateList()
{
    updateTempSets();

    listListW.clear();
    groupListW.clear();
    foreach(QList<QSet<Node*> > set,wSets){
        foreach(QSet<Node*> eachSet, set){
            if(eachSet.count()==1||(eachSet.count()==2&&eachSet.toList().first()->ndum==eachSet.toList().last()->ndum)){
                //for single point
                groupListW<<"["+QString::number(eachSet.toList().first()->ndum)+"]";
            }
            else{
                //for stream
                QList<int> list;
                foreach(Node* node,eachSet){
                    if(!list.contains(node->ndum)){
                        list<<node->ndum;
                    }
                }
                qSort(list.begin(),list.end());
                QStringList sList;
                for(int i = 0; i < list.count();i++){
                    sList<<QString::number(list.at(i));
                }
                groupListW<<"[";
                groupListW.append(sList.join(","));
                groupListW<<"]";
            }

        }
        QString groups = groupListW.join("");
        if(!listListW.contains(groups)){
            listListW.append(groups);
        }
        groupListW.clear();
    }


    listModelW = new QStringListModel;
    listModelW->setStringList(listListW);
    ui->listView->setModel(listModelW);
}

void iwfixDialog::on_addButton_clicked()
{
    QString cb1 = ui->tCB1->currentText();
    cb1.replace("W#","");
    cb1.replace("W-Stream: ","");
    QStringList cbi1 = cb1.split(",");
    QString cb2 = ui->tCB2->currentText();
    cb2.replace("W#","");
    cb2.replace("W-Stream: ","");
    QStringList cbi2 = cb2.split(",");

    if(cbi1.first()==cbi2.first())
    {
        globalpara.reportError("Please add correlation between two different points/streams!",this);
    }
    else
    {
        Node* node1, *node2;
        unit* iterator = dummy;
        for(int x = 0; x < globalcount;x++)
        {
            iterator= iterator->next;
            for(int y = 0; y< iterator->usp;y++)
            {
                if(iterator->myNodes[y]->ndum == cbi1.first().toInt())
                    node1 = iterator->myNodes[y];
                else if(iterator->myNodes[y]->ndum == cbi2.first().toInt())
                    node2 = iterator->myNodes[y];
            }
        }



        bool inGroup1= false, inGroup2 = false, first1 = true, first2 = true;
        QSet<Node*> set1, set2;
        foreach(QSet<Node*> set, globalpara.wGroup)
        {
            if(set.contains(node1))
                if(first1)
                {
                    inGroup1 = true;
                    first1 = false;
                    set1 = set;
                }

            if(set.contains(node2))
                if(first2)
                {
                    inGroup2 = true;
                    first2 = false;
                    set2 = set;
                }
        }

        if(!inGroup1&&!inGroup2)//if neither is in a group, create a new one --linked and inline, add head-up
        {
            QSet<Node*> mset1;
            QSet<Node*> mset2;
            globalpara.allSet.clear();
            node1->searchAllSet("w");
            mset1 = globalpara.allSet;
            globalpara.allSet.clear();
            node2->searchAllSet("w");
            mset2 = globalpara.allSet;
            QSet<Node*> newSet = mset1.unite(mset2);
            globalpara.wGroup.append(newSet);
        }


        else if(inGroup1&&!inGroup2)//unknown one
        {
            globalpara.wGroup.removeOne(set1);
            QSet<Node*> mset2;
            globalpara.allSet.clear();
            node2->searchAllSet("w");
            mset2 = globalpara.allSet;
            set1.unite(mset2);
            globalpara.wGroup.append(set1);
        }

        else if(inGroup2&&!inGroup1)//unknown one
        {
            globalpara.wGroup.removeOne(set2);
            QSet<Node*> mset1;
            globalpara.allSet.clear();
            node1->searchAllSet("w");
            mset1 = globalpara.allSet;
            set2.unite(mset1);
            globalpara.wGroup.append(set2);
        }

        else if(inGroup1&&inGroup2)
        {
            if(set1 == set2)
            {
                globalpara.reportError("The two parameters are already in the same group!");
            }
            else
            {
                QStringList list;
                QString string1, string2;
                foreach(Node* node,set1)
                {
                    if(!list.contains(QString::number(node->ndum)))
                        list.append(QString::number(node->ndum));
                }
                string1.append("Group 1:");
                for(int i = 0; i < list.count();i++)
                    string1.append("sp#"+list[i]+" ");

                list.clear();
                foreach(Node* node,set2)
                {
                    if(!list.contains(QString::number(node->ndum)))
                        list.append(QString::number(node->ndum));
                }
                string2.append("Group 2:");
                for(int i = 0; i < list.count();i++)
                    string2.append("sp#"+list[i]+" ");

                QMessageBox::StandardButton task;

                task = QMessageBox::question(this,"Warning",
                                            "The following two groups will be merged:\n"+string1+"\n"+string2+"\nContinue?",
                                           QMessageBox::Yes|QMessageBox::No);
                if(task == QMessageBox::Yes)
                {
                    globalpara.wGroup.removeOne(set1);
                    globalpara.wGroup.removeOne(set2);
                    set2 = set2.unite(set1);
                    globalpara.wGroup.append(set2);
                }

            }
        }

        else qDebug()<<"what the heck?";
    }



    updateList();
    globalpara.resetIfixes('w');

}


void iwfixDialog::on_removeButton_clicked()
{
    QMessageBox::StandardButton reply;
    bool confirmed;
    reply = QMessageBox::question(this,"Warning",
                                  "Are you sure to remove this group?",
                                  QMessageBox::Yes|QMessageBox::No);
    confirmed = (reply == QMessageBox::Yes);
    if(confirmed)
    {
        QSet<Node*> delSet = globalpara.wGroup.at(ui->listView->currentIndex().row());
        globalpara.wGroup.removeAt(ui->listView->currentIndex().row());
        foreach(Node*node, delSet){
            bool inGroup = false;
            foreach(QSet<Node*> set,globalpara.wGroup){
                if(set.contains(node)){
                    inGroup = true;
                }
            }
            if(!inGroup){
                globalpara.allSet.clear();
                node->searchAllSet("w");
                QSet<Node*> newSet = globalpara.allSet;
                if(newSet.count()==1||(newSet.count()==2&&newSet.toList().first()->linked)){
                    //a lone point
                }
                else{
                    globalpara.addGroup('w',newSet);
                }
            }
            node->iwfix=1;
        }
        globalpara.resetIfixes('w');
    }

    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);
    updateList();

}


void iwfixDialog::on_okButton_clicked()
{
    close();
}



void iwfixDialog::on_modifyButton_clicked()
{
    int index = ui->listView->currentIndex().row();
    QStringList currentGroup = listListW.at(index).split("][");
    foreach(QString string,currentGroup){
        currentGroup.removeOne(string);
        string.replace("]","");
        string.replace("[","");
        currentGroup.append(string);
    }

    QString modifiedString;

    if(ui->listView->currentIndex().row()>=0)
    {
        int currentGroupIndex = ui->listView->currentIndex().row();
        modifiedString= QInputDialog::getItem(this,"Modify group","Choose the state point/stream to remove from group",currentGroup,0,false);

        modifiedString.replace("W#","");
        modifiedString.replace("W-Stream: ","");
        QStringList modifiedString1 = modifiedString.split(",");

        if(!modifiedString1.isEmpty())
        {
            Node* node1;
            unit* iterator = dummy;
            QSet<Node*>set1;
            for(int x = 0; x < globalcount;x++)
            {
                iterator= iterator->next;
                for(int y = 0; y< iterator->usp;y++)
                {
                    if(iterator->myNodes[y]->ndum == modifiedString1.first().toInt())
                    {
                        node1 = iterator->myNodes[y];
                    }
                }
            }
            globalpara.allSet.clear();
            node1->searchAllSet("w");
            set1 = globalpara.allSet;

            QSet<Node*> tempSet = globalpara.wGroup.at(currentGroupIndex);
            globalpara.wGroup.removeOne(tempSet);
            foreach(Node* node,set1){
                tempSet.remove(node);
                node->iwfix=1;
            }
            globalpara.wGroup.append(tempSet);
            if(set1.count()==1||(set1.count()==2&&node1->linked)){
                //a lone point
            }
            else{
                globalpara.addGroup('w',set1);
            }




        }

    }


    globalpara.resetIfixes('w');
    updateList();
    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);

}

void iwfixDialog::on_listView_clicked(const QModelIndex &index)
{
    ui->modifyButton->setEnabled(true);
    ui->removeButton->setEnabled(true);
}

bool iwfixDialog::event(QEvent *e)
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
