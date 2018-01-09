#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QDialog>
#include <QGraphicsSimpleTextItem>
#include <QString>
#include <QFont>
namespace Ui {
class textedit;
}

class SimpleTextItem: public QGraphicsSimpleTextItem
{
public:
    int type() const
    {
        return 10000;
    }
};

class textedit : public QDialog
{
    Q_OBJECT
    
public:
    explicit textedit(QWidget *parent = 0);
    explicit textedit(QWidget *parent,  SimpleTextItem * textitem);
    ~textedit();
    SimpleTextItem * text;
private slots:
    void on_buttonBox_accepted();


private:
    Ui::textedit *ui;
    bool event(QEvent *e);
};



#endif // TEXTEDIT_H
