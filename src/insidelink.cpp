/*! \file insidelink.cpp
    \brief Provides the class insideLink.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

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
