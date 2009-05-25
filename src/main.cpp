#include <time.h>

#include <QApplication>

#include "main.h"

#include "debug.h"
#include "prfconst.h"
#include "kpref.h"
#include "formbid.h"


int main (int argc, char *argv[]) {
#if !defined(WIN32) && !defined(_WIN32)
  char *e = getenv("DEBUG");
  if (!(!e || !strcmp(e, "0") || !strcasecmp(e, "off") || strcasecmp(e, "ona"))) allowDebugLog = 1;
#endif

  for (int f = 1; f < argc; f++) {
    if (!strcmp(argv[f], "-d")) {
      for (int c = f; c < argc; c++) argv[c] = argv[c+1];
      argc--;
      f--;
    }
  }

  qsrand((unsigned)time(0));

  QApplication a(argc, argv);

  //a.setFont (QFont("helvetica", FONTSIZE));
  kpref = new Kpref();
  formBid = new FormBid(kpref);
  formBid->hide();
  kpref->init();
  kpref->show();
  return a.exec();
}
