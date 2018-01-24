//! \file sorputils.cpp
//! \brief Utility functions for common tasks required by SorpSim
//!
//! Copyright (2018) Nicholas Fette
//!
//! \author Nicholas Fette (nfette)

#include "sorputils.h"

QMap<QString, QDomElement> Sorputils::mapElementsByAttribute(const QDomNodeList &nodes, QString attr)
{
    QMap<QString, QDomElement> result;
    for (int i = 0; i < nodes.length(); i++)
    {
        QDomElement el = nodes.at(i).toElement();
        result.insert(el.attribute(attr), el);
    }
    return result;
}

Sorputils::Sorputils()
{

}
