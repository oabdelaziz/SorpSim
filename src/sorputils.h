//! \file sorputils.h
//! \brief Utility functions for common tasks required by SorpSim
//!
//! Copyright (2018) Nicholas Fette
//!
//! \author Nicholas Fette (nfette)

#ifndef SORPUTILS_H
#define SORPUTILS_H

#include <QString>
#include <QMap>
#include <QDomElement>
#include <QDomNodeList>

class Sorputils
{

public:

    //!
    //! \brief mapElementsByAttribute lets you use an attribute as a key
    //!
    //! Assumes that all the nodes in the given list can be cast toElement().
    //!
    //! Typical usage:
    //! myMap = mapElementsByAttribute(el.elementsByTagName("plot"), "title"));
    //! if (myMap.contains("plot_1"))
    //!     QDomElement myPlot = myMap.value("plot_1");
    //!
    //! \return A map from the value of the given attribute to the elements having that attribute value.
    //!
    static QMap<QString, QDomElement> mapElementsByAttribute(const QDomNodeList &, QString attr);

private:
    Sorputils();

};

#endif // SORPUTILS_H
