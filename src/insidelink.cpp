/*insidelink.cpp
 * [SorpSim v1.0 source code]
 * [developed by Zhiyao Yang and Dr. Ming Qu for ORNL]
 * [last updated: 10/12/15]
 *
 * custom class to indicate same stream inside a component (such as in heat exchanger)
 * used to apply flow rate and concentration variable group changes
 * called by editUnitDialog.cpp, mainwindow.cpp, node.cpp
 */





#include "insidelink.h"

insideLink::insideLink(Node*insideNode, Node*regularNode)
{
    myInsideNode = insideNode;
    myRegularNode = regularNode;

    myInsideNode->addInsideLink(this);
    myRegularNode->addInsideLink(this);

}

Node *insideLink::insideNode() const
{
    return myInsideNode;
}

Node *insideLink::regularNode() const
{
    return myRegularNode;
}
