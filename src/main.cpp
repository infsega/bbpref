#include <QApplication>

#include "prfconst.h"
#include "kpref.h"
#include "formbid.h"


int main (int argc, char *argv[]) {
  int res;
  QApplication a(argc, argv);
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
