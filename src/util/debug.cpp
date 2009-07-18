#include <QFile>

#include "debug.h"
#include <cstdlib>
using namespace std;


int allowDebugLog = 0;


#ifdef XDEBUG
void dlogS (const QString &s) {
  QByteArray ba(s.toUtf8());
  dlogf("%s", ba.constData());
}


void dlogf (const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  dlogfVA(fmt, ap);
  va_end(ap);
}


void dlogfVA (const char *fmt, va_list ap) {
  va_list ap2;
  char buf[256];
  char *xbuf = NULL;
  char timebuf[128];
  time_t t;
  struct tm bt;

  if (!fmt || !allowDebugLog) return;

  va_copy(ap2, ap);
  int res = vsnprintf(buf, sizeof(buf)/sizeof(char), fmt, ap2);
  va_end(ap2);
  if (res > (int)(sizeof(buf)/sizeof(char))-1) { /*(int) for c++*/
    xbuf = (char *)malloc((res+32)*sizeof(char));
    if (!xbuf) return;
    va_copy(ap2, ap);
    res = vsnprintf(xbuf, res+31, fmt, ap2);
    va_end(ap2);
  } else xbuf = buf;

  t = time(NULL);
  localtime_r(&t, &bt);
  sprintf(timebuf, "%04i/%02i/%02i %02i:%02i:%02i: ",
    bt.tm_year+1900, bt.tm_mon, bt.tm_mday,
    bt.tm_hour, bt.tm_min, bt.tm_sec
  );

  FILE *outfile = fopen("debug.log", "a");
  if (outfile) {
    fputs(timebuf, outfile);
    char *p = xbuf;
    while (*p) {
      fputc(*p, outfile);
      if (*p == '\n') {
        if (p[1]) fputs(timebuf, outfile);
      }
      p++;
    }
    if (p > xbuf && p[-1] != '\n') fputc('\n', outfile);
    fclose(outfile);
  }

  if (xbuf != buf) free(xbuf);
}
#endif
