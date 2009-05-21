#include "qbidbutton.h"


QBidButton::QBidButton (tGameBid aBid, int x, int y, QWidget *parent) : QPushButton(parent), mBid(aBid) {
  QString iName, oName;
  iName.sprintf(":/pics/bids/s%i.png", aBid);
  oName.sprintf("g%i", aBid);
  setObjectName(oName);
  setGeometry(x, y, 40, 27);
  setMinimumSize(40, 27);
  setIconSize(QSize(40, 27));
  setIcon(QIcon(iName));
}


QBidButton::~QBidButton () {
}
