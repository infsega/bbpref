#include <QObject>

#include "qtformtorg.h"

#include "prfconst.h"


QTFormtorg *Formtorg;


QTFormtorg::QTFormtorg (QWidget *parent) : QDialog (parent) {
  initDialog();
}


QTFormtorg::~QTFormtorg () {
}


void QTFormtorg::slotPushButtonClick86 () { _GamesType = g86; accept(); }
void QTFormtorg::slotPushButtonClickPass () { _GamesType = gtPass; accept(); }
void QTFormtorg::slotPushButtonClickVist () { _GamesType = vist; accept(); }
void QTFormtorg::slotGetBackSnos () { _GamesType = (tGameBid)0; reject(); }
void QTFormtorg::slotShowBullet () { _GamesType = (tGameBid)1; reject(); }


void QTFormtorg::onBidClick () {
  QBidButton *b = dynamic_cast<QBidButton *>(sender());
  if (!b) return;
  _GamesType = b->bid();
  accept();
}


QList<QPushButton *> QTFormtorg::buttonList () {
  QList<QPushButton *> res;
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = dynamic_cast<QPushButton *>(widget);
    if (b) res << b;
  }
  return res;
}


QList<QBidButton *> QTFormtorg::bidButtonList () {
  QList<QBidButton *> res;
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QBidButton *b = dynamic_cast<QBidButton *>(widget);
    if (b) res << b;
  }
  return res;
}


void QTFormtorg::EnableAll (void) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) b->setEnabled(true);
}


void QTFormtorg::DisalbeAll (void) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) b->setEnabled(false);
}


void QTFormtorg::DisableLessThan (tGameBid GamesType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (GamesTypeByName(b->objectName()) < GamesType) b->setEnabled(false);
  }
}


void QTFormtorg::DisalbeGames (tGameBid GamesType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (GamesTypeByName(b->objectName()) == GamesType) b->setEnabled(false);
  }
}


void QTFormtorg::EnableGames (tGameBid GamesType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (GamesTypeByName(b->objectName()) == GamesType) b->setEnabled(true);
  }
}


void QTFormtorg::initDialog () {
  this->resize(220, 230);
  this->setMinimumSize(220, 230);
  this->setMaximumSize(220, 230);

  for (int face = 6; face <= 10; face++) {
    int y = (face-6)*30+10;
    for (int suit = 1; suit <= 5; suit++) {
      int x = (suit-1)*40+10;
      int bid = face*10+suit;
      QBidButton *b = new QBidButton((tGameBid)bid, x, y, this);
      connect(b, SIGNAL(clicked()), this, SLOT(onBidClick()));
    }
  }

  bpass = new QPushButton(this);
  bpass->setGeometry(10,160,70,30);
  bpass->setMinimumSize(0,0);
  bpass->setObjectName("pass");
  bpass->setText("&Pass");

  bvist = new QPushButton(this);
  bvist->setGeometry(80,160,60,30);
  bvist->setMinimumSize(0,0);
  bvist->setObjectName("vist");
  bvist->setText("&Whist");

  b86 = new QPushButton(this);
  b86->setGeometry(140, 160, 70, 30);
  b86->setMinimumSize(0,0);
  b86->setObjectName("g86");
  b86->setText("&Misere");

  showbullet = new QPushButton(this);
  showbullet->setGeometry(110, 190, 100, 27);
  showbullet->setMinimumSize(0, 0);
  showbullet->setText("&Score");

  bgetback = new QPushButton(this);
  bgetback->setGeometry(10, 190, 100, 27);
  bgetback->setMinimumSize(0, 0);
  bgetback->setText("Get &back");

  connect(b86, SIGNAL(clicked()), this, SLOT(slotPushButtonClick86()));
  connect(bpass, SIGNAL(clicked()), this, SLOT(slotPushButtonClickPass()));
  connect(bvist, SIGNAL(clicked()), this, SLOT(slotPushButtonClickVist()));
  connect(bgetback, SIGNAL(clicked()), this, SLOT(slotGetBackSnos()));
  connect(showbullet, SIGNAL(clicked()), this, SLOT(slotShowBullet()));
}
