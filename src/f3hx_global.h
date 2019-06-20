#ifndef F3HX_GLOBAL_H
#define F3HX_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(F3HX_LIBRARY)
#  define F3HXSHARED_EXPORT Q_DECL_EXPORT
#else
#  define F3HXSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // F3HX_GLOBAL_H
