#ifndef DEBUG_H
#define DEBUG_H

#include <QString>

#define XDEBUG


extern int allowDebugLog;


#ifdef XDEBUG
void dlogS (const QString &s);
void dlogf (const char *fmt, ...) __attribute__((format(printf, 1, 2)));
void dlogfVA (const char *fmt, va_list ap);
#else
# define dlogS(...)
# define dlogf(...)
# define dlogfVA(...)
#endif


#endif
