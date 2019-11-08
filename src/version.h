#ifndef VERSION_H
#define VERSION_H

#ifdef SORPVERSION
const QString SORP_VERSION = SORPVERSION;
#else
const QString SORP_VERSION = "Custom build";
#endif

#endif // VERSION_H
