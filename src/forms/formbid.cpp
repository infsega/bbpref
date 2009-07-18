#include <QApplication>
#include <QDesktopWidget>
#include <QObject>
#include <QToolTip>

#include "formbid.h"

#include "prfconst.h"


FormBid *formBid;


FormBid::FormBid (QWidget *parent) : QDialog (parent) {
  this->setModal(true);
  initDialog();
  //bgetback->setToolTip(tr("You lose without 3 takes and no whists on you"));
  showbullet->setToolTip(tr("Show game table with calculated scores"));
}


FormBid::~FormBid () {
}


void FormBid::showEvent (QShowEvent *event) {
  Q_UNUSED(event)
  QWidget *parentObj = dynamic_cast<QWidget *>(parent());
  int x, y;
  if (parentObj) {
    QRect dims(parentObj->frameGeometry());
    x = dims.left()+(dims.width()-width())/2;
    y = dims.top()+(dims.height()-height())/2;
  } else {
    QDesktopWidget *desk = QApplication::desktop();
    QRect dims(desk->availableGeometry(this));
    x = dims.left()+(dims.width()-width())/2;
    y = dims.top()+(dims.height()-height())/2;
  }
  move(x, y);
}


void FormBid::slotPushButtonClick86 () { _GamesType = g86; accept(); }
void FormBid::slotPushButtonClickPass () { _GamesType = gtPass; accept(); }
void FormBid::slotPushButtonClickVist () { _GamesType = whist; accept(); }
void FormBid::slotPushButtonClickHalfVist () { _GamesType = halfwhist; accept(); }
void FormBid::slotGetBackSnos () { _GamesType = (eGameBid)0; reject(); }
void FormBid::slotShowBullet () { _GamesType = (eGameBid)1; reject(); }


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


void FormBid::enableAll () {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) b->setEnabled(true);
}


void FormBid::disableAll () {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) b->setEnabled(false);
}


void FormBid::disableLessThan (eGameBid gameType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (gameName2Type(b->objectName()) < gameType) b->setEnabled(false);
  }
}


void FormBid::disableItem (eGameBid gameType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (gameName2Type(b->objectName()) == gameType) b->setEnabled(false);
  }
}


void FormBid::enableItem (eGameBid gameType) {
  QList<QPushButton *> wList(buttonList());
  foreach (QPushButton *b, wList) {
    if (gameName2Type(b->objectName()) == gameType) b->setEnabled(true);
  }
}


void FormBid::initDialog () {
  this->resize(220, 260);
  this->setMinimumSize(220, 260);
  this->setMaximumSize(220, 260);

  for (int face = 6; face <= 10; face++) {
    int y = (face-6)*30+10;
	if (face > 8)
		y += 30;
    for (int suit = 1; suit <= 5; suit++) {
      int x = (suit-1)*40+10;
      int bid = face*10+suit;
      QBidButton *b = new QBidButton((eGameBid)bid, x, y, this);
      connect(b, SIGNAL(clicked()), this, SLOT(onBidClick()));
    }
  }

  b86 = new QPushButton(this);
  b86->setGeometry(10, 100, 200, 27);
  b86->setMinimumSize(0,0);
  b86->setObjectName("g86");
  b86->setText(tr("&Misere"));
  
  bpass = new QPushButton(this);
  bpass->setGeometry(10,190,60,30);
  bpass->setMinimumSize(0,0);
  bpass->setObjectName("pass");
  bpass->setText(tr("&Pass"));
  bpass->setIcon(QIcon(QString(":/pics/pass.png")));

  bvist = new QPushButton(this);
  bvist->setGeometry(150,190,60,30);
  bvist->setMinimumSize(0,0);
  bvist->setObjectName("whist");
  bvist->setText(tr("&Whist"));
  bvist->setIcon(QIcon(QString(":/pics/whist.png")));

  bhalfvist = new QPushButton(this);
  bhalfvist->setGeometry(70, 190, 80, 30);
  bhalfvist->setMinimumSize(0,0);
  bhalfvist->setObjectName("halfwhist");
  bhalfvist->setText(tr("&HalfWhist"));
  bhalfvist->setIcon(QIcon(QString(":/pics/halfwhist.png")));

  showbullet = new QPushButton(this);
  showbullet->setGeometry(110, 220, 100, 27);
  showbullet->setMinimumSize(0, 0);
  showbullet->setText(tr("S&core"));
  showbullet->setIcon(QIcon(QString(":/pics/paper.png")));

  bgetback = new QPushButton(this);
  bgetback->setGeometry(10, 220, 100, 27);
  bgetback->setMinimumSize(0, 0);
  bgetback->setText(tr("Get &back"));
  bgetback->setIcon(QIcon(QString(":/pics/getback.png")));

  connect(b86, SIGNAL(clicked()), this, SLOT(slotPushButtonClick86()));
  connect(bpass, SIGNAL(clicked()), this, SLOT(slotPushButtonClickPass()));
  connect(bvist, SIGNAL(clicked()), this, SLOT(slotPushButtonClickVist()));
  connect(bhalfvist, SIGNAL(clicked()), this, SLOT(slotPushButtonClickHalfVist()));
  connect(bgetback, SIGNAL(clicked()), this, SLOT(slotGetBackSnos()));
  connect(showbullet, SIGNAL(clicked()), this, SLOT(slotShowBullet()));

}
