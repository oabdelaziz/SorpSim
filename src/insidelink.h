#ifndef INSIDELINK_H
#define INSIDELINK_H

#include "node.h"
#include "unit.h"

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
