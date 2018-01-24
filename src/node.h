#ifndef NODE_H
#define NODE_H
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QString>
#include <QSet>
#include "link.h"
#include <QPainter>
#include <QPointF>
#include "unit.h"
#include <QSet>
#include "insidelink.h"

class Link;
class unit;
class insideLink;

class Node : public QGraphicsItem
{
public:
    Node();
    ~Node();

    void addLink(Link *link);    
    void removeLink(Link *link);

    void addInsideLink(insideLink* insidelink);
    void removeInsideLink();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setHighlighted(bool isHigh);
    void passParaToMerged();
    void passIndToMerged();
    static void mergeInsidePoint(Node*insidePoint, Node*outPoint);
    static void demergeInsidePoint(Node*insidePoint, Node*outPoint);

    QPointF getPosition();

    QGraphicsSimpleTextItem * text;     // Text box to show this node's index on the display
    // QGraphicsSimpleTextItem * parameters;  // Unused. See unit::spParameter. TODO - delete.
    // bool real; // Unused. TODO - delete.

    int ndum;       // Index of this node in global list of nodes
    int ksub;       // Index into fluid inventory (see Link::setColor)
    int itfix;
    float t;        // temperature
    int iffix;
    float f;        // flow rate
    int icfix;
    float c;        // concentration
    int ipfix;
    float p;        // pressure
    int iwfix;
    float w;        // vapor fraction?

    // Calculation results
    float tr;
    float fr;
    float cr;
    float pr;
    float wr;
    float hr;       // probably enthalpy

    // For table mode. Maybe some of these have to do with guess values?
    float tT;
    float fT;
    float cT;
    float pT;
    float wT;

    float tTr;
    float fTr;
    float cTr;
    float pTr;
    float wTr;
    float hTr;

    // Links
    // A link is like a pipe between units. It goes from one node (that should
    // be an outlet for its unit) to another node.
    // If a node is "inside" the unit, any linking should be done with the
    // insideLink class, which differentiates the inside and outside links.
    bool linked;                        // Presumably, whether there is a link from this node to another
    bool insideLinked;
    bool isOutlet;                      // Whether this Node is intended as an outlet for myUnit.
    int unitindex;                      // Copied from parent's unit::nu.
                                        //   Note: set by TreeDialog::on_selectButton_clicked(),
                                        //   MainWindow::deleteunit, loadCase, and loadOutFile.
                                        //   (We could ask unit to be responsible for that...)
    bool isinside;                      // Whether this Node is intended to be internal/inside
                                        //   (a candidate to merge with a corresponding inlet,
                                        //   not to be linked to another unit).
    bool linklowerflag;//means current one is larger
    int localindex;//starting 1
    QSet <Link*> myLinks;               // This is problematic. All client code converts
                                        //   the set to a list and looks at the first element.
                                        //   TODO: change to a (smart) point to Link.
    insideLink * myInsideLink;
    unit* myUnit;                       // Points back to the unit to which this Node belongs.
                                        //   Note: set by myScene::drawAUnit().

    bool isHighlighted;       // For the display, whether the node is highlighted, appearing with a different color.
    bool lineHighlighted;     //

    // These sets are used to convey information about flow through the unit.
    // They represent a directional graph in which target nodes receive
    // flow or state properties, with the same letter abbreviations as above.
    QSet<Node*> FSet;
    QSet<Node*> CSet;
    QSet<Node*> FluidSet;
    QSet<Node*> PSet;
    QSet<Node*> TSet;
    QSet<Node*> WSet;
    // This set is slightly different, in that it does not have an analogous
    // node property. The only known client is editPropertyCurveDialog::updateLoopList.
    QSet<Node*> FCSet;

    void setColor();
    void showFluid(bool toShow);
    void addToSet(Node* node, QString type);
    QSet<Node*> returnSet(QString type);
    bool searchAllSet(QString type, bool searchCloseLoop=false);
    bool addLinkedSet(Node* thisNode, QString type, bool searchCloseLoop=false, Node* startNode=NULL);

private:

    QVariant itemChange (GraphicsItemChange change,
                         const QVariant &value);
    QRectF boundingRect() const;

    QColor myBackgroundColor;
    QColor myOutlineColor;



};

#endif // NODE_H
