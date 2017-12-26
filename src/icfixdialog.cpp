/*icfixdialog.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * dialog to edit concentration variable groups
 * called by vicheckdialog.cpp
 */




#include "icfixdialog.h"
#include "ui_icfixdialog.h"
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
extern QSet<Node*> spOnline;
extern MainWindow*theMainwindow;
extern VICheckDialog*theVIDialog;

QStringList groupListC;
QStringList listListC;
QStringListModel * listModelC;

icfixDialog::icfixDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::icfixDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Tool);
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle("Concentration");
    ui->label->setText("Please define state points that have same concentration.");

    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    globalpara.resetIfixes('c');


    QStringList cbListC;
    QStringList tempListC;
    unit * iterator = dummy;
    for(int i = 0; i<globalcount;i++)
    {
        iterator= iterator->next;

        for(int j = 0; j<iterator->usp;j++)
        {
            if(iterator->myNodes[j]->icfix>0)
            {
                Node* theNode = iterator->myNodes[j];
                QSet<Node*>cList;
                globalpara.allSet.clear();
                theNode->searchAllSet("c");
                cList = globalpara.allSet;

                if((theNode->linked&&cList.count()==2&&theNode==theNode->myLinks.toList().first()->myToNode)
                        ||(!theNode->linked&&cList.count()==1)){
//single point
                    if(!cbListC.contains("C#"+QString::number(theNode->ndum))){
                    cbListC.append("C#"+QString::number(theNode->ndum));

                    }

                }
                else if(!(theNode->linked&&cList.count()==2&&theNode==theNode->myLinks.toList().first()->myFromNode)){
//in a stream
                    tempListC.clear();
                    QList<int> list;
                    for(int i = 0; i < cList.count(); i++){
                        Node * node = cList.toList().at(i);
                        if(!list.contains(node->ndum)){
                            list.append(node->ndum);
                        }
                    }
                    qSort(list.begin(),list.end());

                    for(int i = 0; i < list.count(); i++){
                        tempListC.append(QString::number(list.at(i)));
                    }


                    if(!cbListC.contains("C-Stream: "+tempListC.join(","))){
                        cbListC.append("C-Stream: "+tempListC.join(","));
                }

                }

            }
        }
    }

    ui->tCB1->addItems(cbListC);
    ui->tCB2->addItems(cbListC);

    updateList();

}

icfixDialog::~icfixDialog()
{
    delete ui;
}

void icfixDialog::updateTempSets()
{
    globalpara.resetIfixes('c');
    cSets.clear();
    QSet<Node*> scanSet;
    for(int n = 0; n < globalpara.cGroup.count();n++){
        QSet<Node*> tempSet = globalpara.cGroup.at(n);
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
                node->searchAllSet("c");
                scanSet = globalpara.allSet;
                newBigSet.append(scanSet);
            }

        }
        cSets.append(newBigSet);
    }
}

void icfixDialog::updateList()
{
    updateTempSets();

    listListC.clear();
    groupListC.clear();
    foreach(QList<QSet<Node*> > set,cSets){
        foreach(QSet<Node*> eachSet, set){
            if(eachSet.count()==1||(eachSet.count()==2&&eachSet.toList().first()->ndum==eachSet.toList().last()->ndum)){
                //for single point
                groupListC<<"["+QString::number(eachSet.toList().first()->ndum)+"]";
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
                groupListC<<"[";
                groupListC.append(sList.join(","));
                groupListC<<"]";
            }

        }
        QString groups = groupListC.join("");
        if(!listListC.contains(groups)){
            listListC.append(groups);
        }
        groupListC.clear();
    }

    listModelC = new QStringListModel;
    listModelC->setStringList(listListC);
    ui->listView->setModel(listModelC);
}

void icfixDialog::on_addButton_clicked()
{
    QString cb1 = ui->tCB1->currentText();
    cb1.replace("C#","");
    cb1.replace("C-Stream: ","");
    QStringList cbi1 = cb1.split(",");
    QString cb2 = ui->tCB2->currentText();
    cb2.replace("C#","");
    cb2.replace("C-Stream: ","");
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
        foreach(QSet<Node*> set, globalpara.cGroup)
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
            node1->searchAllSet("c");
            mset1 = globalpara.allSet;
            globalpara.allSet.clear();
            node2->searchAllSet("c");
            mset2 = globalpara.allSet;
            QSet<Node*> newSet = mset1.unite(mset2);
            globalpara.cGroup.append(newSet);
        }


        else if(inGroup1&&!inGroup2)//unknown one
        {
            globalpara.cGroup.removeOne(set1);
            QSet<Node*> mset2;
            globalpara.allSet.clear();
            node2->searchAllSet("c");
            mset2 = globalpara.allSet;
            set1.unite(mset2);
            globalpara.cGroup.append(set1);
        }

        else if(inGroup2&&!inGroup1)//unknown one
        {
            globalpara.cGroup.removeOne(set2);
            QSet<Node*> mset1;
            globalpara.allSet.clear();
            node1->searchAllSet("c");
            mset1 = globalpara.allSet;
            set2.unite(mset1);
            globalpara.cGroup.append(set2);
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
                    globalpara.cGroup.removeOne(set1);
                    globalpara.cGroup.removeOne(set2);
                    set2 = set2.unite(set1);
                    globalpara.cGroup.append(set2);
                }

            }
        }

        else qDebug()<<"what the heck?";
    }



    updateList();
    globalpara.resetIfixes('c');
}


void icfixDialog::on_removeButton_clicked()
{
    QMessageBox::StandardButton reply;
    bool confirmed;
    reply = QMessageBox::question(this,"Warning",
                                  "Are you sure to remove this group?",
                                  QMessageBox::Yes|QMessageBox::No);
    confirmed = (reply == QMessageBox::Yes);
    if(confirmed)
    {
        QSet<Node*> delSet = globalpara.cGroup.at(ui->listView->currentIndex().row());
        globalpara.cGroup.removeAt(ui->listView->currentIndex().row());
        foreach(Node*node, delSet){
            bool inGroup = false;
            foreach(QSet<Node*> set,globalpara.cGroup){
                if(set.contains(node)){
                    inGroup = true;
                }
            }
            if(!inGroup){
                globalpara.allSet.clear();
                node->searchAllSet("c");
                QSet<Node*> newSet = globalpara.allSet;
                if(newSet.count()==1||(newSet.count()==2&&newSet.toList().first()->linked)){
                    //a lone point
                }
                else{
                    globalpara.addGroup('c',newSet);
                }
            }
            node->icfix=1;
        }
        globalpara.resetIfixes('c');
    }
    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);
    updateList();

}


void icfixDialog::on_okButton_clicked()
{
    close();
}


void icfixDialog::on_modifyButton_clicked()
{
    int index = ui->listView->currentIndex().row();
    QStringList currentGroup = listListC.at(index).split("][");
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

        modifiedString.replace("C#","");
        modifiedString.replace("C-Stream: ","");
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
            node1->searchAllSet("c");
            set1 = globalpara.allSet;

            QSet<Node*> tempSet = globalpara.cGroup.at(currentGroupIndex);
            globalpara.cGroup.removeOne(tempSet);
            foreach(Node* node,set1){
                tempSet.remove(node);
                node->icfix=1;
            }
            globalpara.cGroup.append(tempSet);
            if(set1.count()==1||(set1.count()==2&&node1->linked)){
                //a lone point
            }
            else{
                globalpara.addGroup('c',set1);
            }




        }

    }


    globalpara.resetIfixes('c');
    updateList();
    ui->modifyButton->setDisabled(true);
    ui->removeButton->setDisabled(true);

}

void icfixDialog::on_listView_clicked(const QModelIndex &index)
{
    ui->modifyButton->setEnabled(true);
    ui->removeButton->setEnabled(true);
}

bool icfixDialog::event(QEvent *e)
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
