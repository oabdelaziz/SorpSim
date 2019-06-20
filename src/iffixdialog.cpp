/*! \file iffixdialog.cpp
    \brief Provides the class iffixDialog.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#include "iffixdialog.h"
#include "ui_iffixdialog.h"
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

QStringList groupListF;
QStringList listListF;
QStringListModel * listModelF;

iffixDialog::iffixDialog(QWidget *parent) :
    QDialog(parent),

    ui(new Ui::iffixDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Mass Flow Rate");
    ui->label->setText("Please define state points that have same mass flow rate.");

    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    globalpara.resetIfixes('f');


    QStringList cbListF;
    QStringList tempListF;
    unit * iterator = dummy;
    for(int i = 0; i<globalcount;i++)
    {
        iterator= iterator->next;

        for(int j = 0; j<iterator->usp;j++)
        {
            if(iterator->myNodes[j]->iffix>0)
            {
                Node* theNode = iterator->myNodes[j];
                QSet<Node*>fList;
                globalpara.allSet.clear();
                theNode->searchAllSet("f");
                fList = globalpara.allSet;

                if((theNode->linked&&fList.count()==2&&theNode==theNode->myLinks.toList().first()->myToNode)
                        ||(!theNode->linked&&fList.count()==1)){
//single point
                    if(!cbListF.contains("F#"+QString::number(theNode->ndum))){
                    cbListF.append("F#"+QString::number(theNode->ndum));

                    }

                }
                else if(!(theNode->linked&&fList.count()==2&&theNode==theNode->myLinks.toList().first()->myFromNode)){
//in a stream
                    tempListF.clear();
                    QList<int> list;
                    for(int i = 0; i < fList.count(); i++){
                        Node * node = fList.toList().at(i);
                        if(!list.contains(node->ndum)){
                            list.append(node->ndum);
                        }
                    }
                    qSort(list.begin(),list.end());

                    for(int i = 0; i < list.count(); i++){
                        tempListF.append(QString::number(list.at(i)));
                    }


                    if(!cbListF.contains("F-Stream: "+tempListF.join(","))){
                        cbListF.append("F-Stream: "+tempListF.join(","));
                }

                }

            }
        }
    }



    ui->tCB1->addItems(cbListF);
    ui->tCB2->addItems(cbListF);

    updateList();

}

iffixDialog::~iffixDialog()
{
    delete ui;
}

void iffixDialog::updateTempSets()
{

    globalpara.resetIfixes('f');
    fSets.clear();
    QSet<Node*> scanSet;
    for(int n = 0; n < globalpara.fGroup.count();n++){
        QSet<Node*> tempSet = globalpara.fGroup.at(n);
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
                node->searchAllSet("f");
                scanSet = globalpara.allSet;
                newBigSet.append(scanSet);
            }

        }
        fSets.append(newBigSet);
    }
}

void iffixDialog::updateList()
{
    updateTempSets();

    listListF.clear();
    groupListF.clear();
    foreach(QList<QSet<Node*> > set,fSets){
        foreach(QSet<Node*> eachSet, set){
            if(eachSet.count()==1||(eachSet.count()==2&&eachSet.toList().first()->ndum==eachSet.toList().last()->ndum)){
                //for single point
                groupListF<<"["+QString::number(eachSet.toList().first()->ndum)+"]";
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
                groupListF<<"[";
                groupListF.append(sList.join(","));
                groupListF<<"]";
            }

        }
        QString groups = groupListF.join("");
        if(!listListF.contains(groups)){
            listListF.append(groups);
        }
        groupListF.clear();
    }


    listModelF = new QStringListModel;
    listModelF->setStringList(listListF);
    ui->listView->setModel(listModelF);
}

void iffixDialog::on_addButton_clicked()
{

    QString cb1 = ui->tCB1->currentText();
    cb1.replace("F#","");
    cb1.replace("F-Stream: ","");
    QStringList cbi1 = cb1.split(",");
    QString cb2 = ui->tCB2->currentText();
    cb2.replace("F#","");
    cb2.replace("F-Stream: ","");
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
        foreach(QSet<Node*> set, globalpara.fGroup)
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
            node1->searchAllSet("f");
            mset1 = globalpara.allSet;
            globalpara.allSet.clear();
            node2->searchAllSet("f");
            mset2 = globalpara.allSet;
            QSet<Node*> newSet = mset1.unite(mset2);
            globalpara.fGroup.append(newSet);
        }


        else if(inGroup1&&!inGroup2)//unknown one
        {
            globalpara.fGroup.removeOne(set1);
            QSet<Node*> mset2;
            globalpara.allSet.clear();
            node2->searchAllSet("f");
            mset2 = globalpara.allSet;
            set1.unite(mset2);
            globalpara.fGroup.append(set1);
        }

        else if(inGroup2&&!inGroup1)//unknown one
        {
            globalpara.fGroup.removeOne(set2);
            QSet<Node*> mset1;
            globalpara.allSet.clear();
            node1->searchAllSet("f");
            mset1 = globalpara.allSet;
            set2.unite(mset1);
            globalpara.fGroup.append(set2);
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
                    globalpara.fGroup.removeOne(set1);
                    globalpara.fGroup.removeOne(set2);
                    set2 = set2.unite(set1);
                    globalpara.fGroup.append(set2);
                }

            }
        }

        else qDebug()<<"what the heck?";
    }



    updateList();
    globalpara.resetIfixes('f');
}


void iffixDialog::on_removeButton_clicked()
{
    QMessageBox::StandardButton reply;
    bool confirmed;
    reply = QMessageBox::question(this,"Warning",
                                  "Are you sure to remove this group?",
                                  QMessageBox::Yes|QMessageBox::No);
    confirmed = (reply == QMessageBox::Yes);
    if(confirmed)
    {
        QSet<Node*> delSet = globalpara.fGroup.at(ui->listView->currentIndex().row());
        globalpara.fGroup.removeAt(ui->listView->currentIndex().row());
        foreach(Node*node, delSet){
            bool inGroup = false;
            foreach(QSet<Node*> set,globalpara.fGroup){
                if(set.contains(node)){
                    inGroup = true;
                }
            }
            if(!inGroup){
                globalpara.allSet.clear();
                node->searchAllSet("f");
                QSet<Node*> newSet = globalpara.allSet;
                if(newSet.count()==1||(newSet.count()==2&&newSet.toList().first()->linked)){
                    //a lone point
                }
                else{
                    globalpara.addGroup('f',newSet);
                }
            }
            node->iffix=1;
        }
        globalpara.resetIfixes('f');
    }
    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);
    updateList();

}


void iffixDialog::on_okButton_clicked()
{
    close();
}


void iffixDialog::on_modifyButton_clicked()
{
    int index = ui->listView->currentIndex().row();
    QStringList currentGroup = listListF.at(index).split("][");
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

        modifiedString.replace("F#","");
        modifiedString.replace("F-Stream: ","");
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
            node1->searchAllSet("f");
            set1 = globalpara.allSet;

            QSet<Node*> tempSet = globalpara.fGroup.at(currentGroupIndex);
            globalpara.fGroup.removeOne(tempSet);
            foreach(Node* node,set1){
                tempSet.remove(node);
                node->iffix=1;
            }
            globalpara.fGroup.append(tempSet);
            if(set1.count()==1||(set1.count()==2&&node1->linked)){
                //a lone point
            }
            else{
                globalpara.addGroup('f',set1);
            }




        }

    }


    globalpara.resetIfixes('f');
    updateList();
    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);

}

void iffixDialog::on_listView_clicked(const QModelIndex &index)
{
    ui->modifyButton->setEnabled(true);
    ui->removeButton->setEnabled(true);
}

bool iffixDialog::event(QEvent *e)
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
