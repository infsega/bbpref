/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
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
#include "deskview.h"

static eGameBid gameName2Type (const QString s) {
  if (s == "raspass") return raspass;
  if (s == "whist") return whist;
  if (s == "undefined") return undefined;
  if (s == "pass") return gtPass;
  if (s == "g61") return g61;
  if (s == "g62") return g62;
  if (s == "g63") return g63;
  if (s == "g64") return g64;
  if (s == "g65") return g65;
  if (s == "g71") return g71;
  if (s == "g72") return g72;
  if (s == "g73") return g73;
  if (s == "g74") return g74;
  if (s == "g75") return g75;
  if (s == "g81") return g81;
  if (s == "g82") return g82;
  if (s == "g83") return g83;
  if (s == "g84") return g84;
  if (s == "g85") return g85;
  if (s == "g86") return g86;
  if (s == "g91") return g91;
  if (s == "g92") return g92;
  if (s == "g93") return g93;
  if (s == "g94") return g94;
  if (s == "g95") return g95;
  if (s == "g101") return g101;
  if (s == "g102") return g102;
  if (s == "g103") return g103;
  if (s == "g104") return g104;
  if (s == "g105") return g105;
  return undefined;
}

namespace {
  class QBidButton : public QPushButton
  {
  public:
    QBidButton (eGameBid aBid, int x, int y, QWidget *parent=0);
    eGameBid bid () const { return mBid; }

  protected:
    eGameBid mBid;
  };

  QBidButton::QBidButton (eGameBid aBid, int x, int y, QWidget *parent) : QPushButton(parent), mBid(aBid) {
      QString iName, oName;
    iName.sprintf(":/pics/bids/s%i.png", aBid);
    oName.sprintf("g%i", aBid);
    setObjectName(oName);
    setGeometry(x, y, 40, 27);
    setMinimumSize(40, 27);
    setIconSize(QSize(40, 27));
    setIcon(QIcon(iName));
  }
} // end of namespace


FormBid::FormBid (DeskView *parent) : QDialog (parent) {
  //this->setModal(true);
  m_deskview = parent;
  setWindowTitle(tr("Bidding"));
  initDialog();
}


FormBid::~FormBid () {
}

FormBid* FormBid::instance(DeskView *parent)
{
  static FormBid *obj = 0;

    if (!obj)
      obj = new FormBid(parent);

    return obj;
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

void FormBid::slotShowBullet ()
{
  m_deskview->drawPool();
}


void FormBid::onBidClick () {
  QBidButton *b = static_cast<QBidButton *>(sender());
  if (!b) return;
  _GamesType = b->bid();
  accept();
}


QList<QPushButton *> FormBid::buttonList () {
  QList<QPushButton *> res;
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = static_cast<QPushButton *>(widget);
    if (b) res << b;
  }
  return res;
}

/*
QList<QPushButton *> FormBid::bidButtonList () {
  QList<QPushButton *> res;
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QBidButton *b = static_cast<QBidButton *>(widget);
    if (b) res << b;
  }
  return res;
}*/


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
