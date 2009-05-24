#include <QApplication>

#include "main.h"

#include "debug.h"
#include "prfconst.h"
#include "kpref.h"
#include "formbid.h"


int main (int argc, char *argv[]) {
  int res;
  QApplication a(argc, argv);

#if !defined(WIN32) && !defined(_WIN32)
  char *e = getenv("DEBUG");
  if (!(!e || !strcmp(e, "0") || !strcasecmp(e, "off") || strcasecmp(e, "ona"))) allowDebugLog = 1;
#endif

  //a.setFont (QFont("helvetica", FONTSIZE));
  kpref = new Kpref();
  //!!!qApp->setMainWidget(kpref);
  formBid = new FormBid(kpref);
  formBid->hide();
  kpref->init();
  //kpref->showMaximized();
  //!!!kpref-> resize(QApplication::desktop()->width()-30, QApplication::desktop()->height()-100);
  //kpref->adjustDesk();
  kpref->show();
  res = a.exec();
  //delete formBid;
  return res;
}
