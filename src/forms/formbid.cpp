/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see file COPYING); if not, see 
 *      http://www.gnu.org/licenses 
 */

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QObject>
#include <QToolTip>

#include "formbid.h"

#include "prfconst.h"


FormBid *formBid;


FormBid::FormBid (QWidget *parent) : QDialog (parent) {
  this->setModal(true);
  setWindowTitle(tr("Bidding"));
  initDialog();
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

void FormBid::closeEvent(QCloseEvent *event) {
	int ret = QMessageBox::question(this, tr("OpenPref"),
        tr("Do you really want to quit the game?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No | QMessageBox::Escape);
	if (ret == QMessageBox::Yes)
         exit(0);
    else
         event->ignore();
}

void FormBid::slotPushButtonClick86 () { _GamesType = g86; accept(); }
void FormBid::slotPushButtonClickPass () { _GamesType = gtPass; accept(); }
void FormBid::slotPushButtonClickVist () { _GamesType = whist; accept(); }
void FormBid::slotPushButtonClickHalfVist () { _GamesType = halfwhist; accept(); }
void FormBid::slotWithoutThree () { _GamesType = withoutThree; accept(); }
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
	  if (suit == 5)
	  	b->setToolTip(tr("no trump"));
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
  showbullet->setGeometry(116, 220, 94, 27);
  showbullet->setMinimumSize(0, 0);
  showbullet->setText(tr("S&core"));
  showbullet->setToolTip(tr("Show game table with calculated scores"));
  showbullet->setIcon(QIcon(QString(":/pics/paper.png")));

  bwithoutthree = new QPushButton(this);
  bwithoutthree->setGeometry(10, 220, 106, 27);
  bwithoutthree->setMinimumSize(0, 0);
  bwithoutthree->setText(tr("Without &Three"));
  bwithoutthree->setToolTip(tr("Cancel game with penalty of three untaken tricks. No whists are written"));
  bwithoutthree->setIcon(QIcon(QString(":/pics/cancel.png")));

  connect(b86, SIGNAL(clicked()), this, SLOT(slotPushButtonClick86()));
  connect(bpass, SIGNAL(clicked()), this, SLOT(slotPushButtonClickPass()));
  connect(bvist, SIGNAL(clicked()), this, SLOT(slotPushButtonClickVist()));
  connect(bhalfvist, SIGNAL(clicked()), this, SLOT(slotPushButtonClickHalfVist()));
  connect(bwithoutthree, SIGNAL(clicked()), this, SLOT(slotWithoutThree()));
  connect(showbullet, SIGNAL(clicked()), this, SLOT(slotShowBullet()));

}
