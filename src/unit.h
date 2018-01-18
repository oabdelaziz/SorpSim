#ifndef UNIT_H
#define UNIT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QVariant>
#include "arrow.h"
#include "airarrow.h"
#include <QGraphicsEllipseItem>
#include "node.h"
#include "qgraphicsarc.h"

class Node;

/**
 * @brief An object of the unit class represents a component-level unit of a sorption system.
 *
 * Typical usage:
 *     unit * myUnit = new unit();
 *     myUnit.idunit = 11; // ABSORBER
 *     myUnit.initialize();
 *     // Populate desired data fields
 *     ...
 *     myUnit.drawUnit();
 *     unit->setParentItem(rect);
 *     // Graphics happens ...
 *     if (done)  {
 *         delete myUnit;
 *     } else {
 *         // Do more complicated things
 *         myScene::addLink(...);
 *         // Clean up
 *         mainWindow::deleteunit(...);
 *     }
 *
 * Note: it is not a QObject, however as QGraphicsItem, it also will destroy
 * children when it is destroyed.
 */
class unit : public QGraphicsItem
{
public:
    unit();
    ~unit();

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    QPointF getPos();
    void initialize();                  // Lazy constructor
    void drawUnit();                    // Very lazy constructor
    void horizontalFlip();
    void verticalFlip();
    void rotateClockWise();
    void updateRotate();
    QTransform flip(QGraphicsItem* item, bool horizontally);

    int nu;         // Unit index in the list (compare Node::unitindex)
    int usp;        // Count of state points (Nodes) used by this unit (from 1 to 7).
    int idunit;     // Type code (used like enum), eg. 62 = throttle valve, 63 = thermostatic valve, etc.

    // Heat transfer variables.
    int iht;        // Heat transfer specification mode: (see manual for detail)
                    //   0 = heat transfer rate
                    //   1 = UA
                    //   2 = NTU
                    //   3 = EFF
                    //   4 = CAT
                    //   5 = LMTD
                    //   6 = skip
                    //   Special case: for splitter (type 81 and 82),
                    //   0 = split ratio not fixed
                    //   1 = split ratio fixed
                    //   For valve, somehow gets set to 3 "to avoid unit conversion".
    float ht;       // Value of heat transfer variable (per iht):
                    //   Special cases: for mixer (id==71||id==72) and valve (id==61||id==62||id==63),
                    //   variable has no meaning and is ignored.
                    //   For splitter (id==81||id==82), if split ratio is fixed, it is the split ratio.
                    //   For throttle valve (id==62), it means the coefficient for the power law.
    int ipinch;     // For heat exchanging streams, mode for hint to locate temperature pinch:
                    //   HotEnd = 1
                    //   ColdEnd = -1
                    //   progDecide = 0
    float devl;     // Degree of subcooling (deviation from sat. liquid). Special cases:
                    //   For throttle valve (type 62), it means the exponent in the power law.
                    //   For thermostatic valve (type 63), it means the sensor node (?).
    float devg;     // Degree of superheating (deviation from sat. vapor). Special cases:
                    //   For thermostatic valve (type 63), it means the incremental temperature added to the sensor node.
    int icop;       // Toggle to use the heat transfer (ht) from this unit to calculate global COP and capacity.
                    //   -1 = Add to denominator of COP
                    //   0  = Do not use
                    //   1  = Add to system capacity and numerator of COP

    // Heat and mass transfer variables for liquid desiccant units.
    double NTUm;    // In liquid desiccant units, (id > 160 && id < 200), this variable applies.
                    //   It is the NTU for mass transfer between desiccant and air streams.
    double NTUt;    // In heated or cooled liquid desiccant units (id from 170 to 179 or 190 to 199),
                    //   this variable applies. It is the NTU for heat transfer between desiccant stream
                    //   and the cooling or heating medium.
                    //   (Note -- called NTUw in the dialog for those components)
    double NTUa;    // In heated or cooled liquid desiccant units (id from 170 to 179 and 190 to 199),
                    //   if wetness is less than 1, this variable applies. It means the NTU
                    //   for heat transfer between air and the cooling or heating medium.
    double wetness; // In heated or cooled liquid desiccant, this indicates
                    //   wetness level or surface wetability, from 0 to 1.

    // Etc.
    bool insideMerged; // For components where phase change occurs, this indicates whether to ignore
                       //   the possibility that inlet and saturation point may be distinct.
                       //   This affects node numbering, and maybe even calculation (how?).
    int nIter;      // For finite difference methods, number of spatial subdivisions
                    //   (probably in the direction of flow of desiccant -- number of subdivisions
                    //   for cross-flowing air stream appears to be fixed at 50.)
    double le;      // For liquid desiccant units, Lewis number for air.

    // Calculated results
    float htr;      // Heat rate
    int ipinchr;    // Unused (TODO - delete or set from results)
    float ua;       // Heat exchange UA
    float ntu;      // Heat exchange NTU
    float eff;      // Heat exchange EFF
    float cat;      // Heat exchange CAT
    float lmtd;     // Heat exchange LMTD
    float mrate;    // Mass exchange rate
    float humeff;   // For desiccant units, Humidity effictiveness on air side, (wai-wao)/(wai-w_sat_in).
    float enthalpyeff; // For desiccant units, Enthalpy effectiveness on air side, (hai-hao)/(hai-h_sat_in).

    // Parametric table mode inputs
    // Evidently, a side effect of opening a parametric table is
    // the values are copied from above to here, plus T at the end of the name.
    // These are then the defaults for table calculations, with table columns
    // overriding the defaults.
    //int ihtT;       // unused -- because why let user control this in the table?
    float htT;        //
    int ipinchT;      //
    int icopT;        //
    double NTUmT;     //
    double NTUtT;     //
    double NTUaT;     //
    double wetnessT;  //
    double leT;       //

    // Parametric table mode outputs from calculation.
    float htTr;           // as in htr (heat transfer rate) + T (table) + (dyslexia)
    int ipinchTr;         //
    float uaT;            //
    float ntuT;           //
    float effT;           //
    float catT;           //
    float lmtdT;          //
    float mrateT;         //
    float humeffT;        //
    float enthalpyeffT;   //


    Node* myNodes[7];                // All the child nodes of this unit.
    unit* next;                      // Pointer to next sibling in the list containing this.
    Node*sensor;                     // For thermostatic valve, pointer to the node for reading temperature.
    QString unitName;                // Eg. "ABOSRBER"
    QString description;             // Longer blurb that appears in dialog box for add new unit.
    QGraphicsSimpleTextItem *utext;  // Eg. "<ABSORBER>", label to put near the unit in the display.
    int mergedOutPoint;              // If there is an internal state point that can be ignored
                                     //   via the insideMerged toggle, then this is the index
                                     //   of the inlet/outlet point with which to merge it.
                                     //   Note: for this index, counting starts at 1,
                                     //   so values <= 0 and > usp are meaningless.

    QGraphicsSimpleTextItem *spParameter[7];  // Text boxes to show state point info on the display (show results).
    QGraphicsSimpleTextItem * unitParameter;  // Text box to show results for this unit on the display.

private:


    QRectF boundingRect() const;

    QGraphicsRectItem * rectangle;
    arrow * myArrow[6];
    QGraphicsLineItem * line1;
    QGraphicsLineItem * line2;
    QGraphicsLineItem * line3;
    QGraphicsLineItem * line4;
    QGraphicsLineItem * line5;
    QGraphicsLineItem * line6;
    QGraphicsLineItem * line7;
    QGraphicsLineItem * line8;
    QGraphicsLineItem * line9;
    QGraphicsLineItem * line0;
    QGraphicsSimpleTextItem * Q;
    QGraphicsEllipseItem *circle;
    QGraphicsPolygonItem * poli;
    QGraphicsEllipseItem *selectedsign;






};

#endif // UNIT_H
