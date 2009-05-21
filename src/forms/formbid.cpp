#include <QApplication>
#include <QDesktopWidget>
#include <QObject>

#include "formbid.h"

#include "prfconst.h"


FormBid *formBid;


FormBid::FormBid (QWidget *parent) : QDialog (parent) {
  initDialog();
}


FormBid::~FormBid () {
}


void FormBid::showEvent (QShowEvent *event) {
  Q_UNUSED(event)
  QDesktopWidget *desk = QApplication::desktop();
  QRect dims(desk->availableGeometry(this));
  int x = dims.left()+(dims.width()-width())/2;
  int y = dims.top()+(dims.height()-height())/2;
  move(x, y);
}


void FormBid::slotPushButtonClick86 () { _GamesType = g86; accept(); }
void FormBid::slotPushButtonClickPass () { _GamesType = gtPass; accept(); }
void FormBid::slotPushButtonClickVist () { _GamesType = vist; accept(); }
void FormBid::slotGetBackSnos () { _GamesType = (tGameBid)0; reject(); }
void FormBid::slotShowBullet () { _GamesType = (tGameBid)1; reject(); }


void FormBid::onBidClick () {
  QBidButton *b = dynamic_cast<QBidButton *>(sender());
  if (!b) return;
  _GamesType = b->bid();
  accept();
}


QList<QPushButton *> FormBid::buttonList () {
  QList<QPushButton *> res;
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = dynamic_cast<QPushButton *>(widget);
    if (b) res << b;
  }
  return res;
}


QList<QBidButton *> FormBid::bidButtonList () {
  QList<QBidButton *> res;
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QBidButton *b = dynamic_cast<QBidButton *>(widget);
    if (b) res << b;
  }
  return res;
}


void FormBid::EnableAll (void) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) b->setEnabled(true);
}


void FormBid::DisalbeAll (void) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) b->setEnabled(false);
}


void FormBid::DisableLessThan (tGameBid GamesType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (GamesTypeByName(b->objectName()) < GamesType) b->setEnabled(false);
  }
}


void FormBid::DisalbeGames (tGameBid GamesType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (GamesTypeByName(b->objectName()) == GamesType) b->setEnabled(false);
  }
}


void FormBid::EnableGames (tGameBid GamesType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (GamesTypeByName(b->objectName()) == GamesType) b->setEnabled(true);
  }
}


void FormBid::initDialog () {
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
