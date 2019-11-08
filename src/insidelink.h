/*! \file insidelink.h
    \brief Provides the class insideLink.

    This file is part of SorpSim and is distributed under terms in the file LICENSE.

    Developed by Zhiyao Yang and Dr. Ming Qu for ORNL.

    \author Zhiyao Yang (zhiyaoYang)
    \author Dr. Ming Qu

    \copyright 2015, UT-Battelle, LLC

*/

#ifndef INSIDELINK_H
#define INSIDELINK_H

#include "node.h"
#include "unit.h"

/*!
 * Custom class to indicate same stream inside a component (such as in heat exchanger)
 * - used to apply flow rate and concentration variable group changes
 * - called by editUnitDialog.cpp, mainwindow.cpp, node.cpp
 */
class insideLink
{
public:
    insideLink(Node*insideNode, Node*regularNode);

    Node* insideNode()const;
    Node* regularNode()const;

private:
    Node* myInsideNode;
    Node* myRegularNode;
};

#endif // INSIDELINK_H
