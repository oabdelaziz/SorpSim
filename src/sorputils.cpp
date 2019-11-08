//! \file sorputils.cpp
//! \brief Utility functions for common tasks required by SorpSim
//!
//! Copyright (2018) Nicholas Fette
//!
//! \author Nicholas Fette (nfette)

#include "sorputils.h"

#include <QCoreApplication>
#include <QFileDevice>

#ifdef Q_OS_WIN32
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;
#endif
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup;

void Sorputils::init()
{
#ifdef Q_OS_WIN32
    // After copying files from QResources, may want to enable WriteGroup permissions
    // See documentation for QFileDevice
    qt_ntfs_permission_lookup++; // turn checking on
#endif
}

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

QDir Sorputils::sorpResourceDir()
{
    QDir result;
    result = QCoreApplication::applicationDirPath();

#if defined Q_OS_MACOS
    result.cd("../Resources");
#elif defined Q_OS_WIN32
    // No change for windows
    ;
#else
    // No change for unix, etc.
    ;
#endif
    return result;
}

QDir Sorputils::sorpTempDir()
{
#if defined Q_OS_MACOS
    QDir result = sorpResourceDir();
    if (!result.exists("temp"))
        result.mkdir("temp");
    result.cd("temp");
    return result;
#else
    return QDir();
#endif
}

QString Sorputils::sorpSettings()
{
    return sorpResourceDir().absoluteFilePath("settings/systemSetting.xml");
}

QString Sorputils::sorpLog()
{
    return sorpResourceDir().absoluteFilePath("log");
}

Sorputils::Sorputils()
{

}
