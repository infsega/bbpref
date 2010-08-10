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
#include <QDebug>

#include "formbid.h"
#include "prfconst.h"
#include "deskview.h"

static eGameBid gameName2Type (const QString s) {
  if (s == "raspass") return raspass;
  if (s == "whist") return whist;
  if (s == "halfwhist") return halfwhist;
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
    void enable();
    void disable();

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

  void QBidButton::enable() {
      if(!isEnabled()) {
          setEnabled(true);
          setFlat(false);
      }
  }
  void QBidButton::disable() {
      if(isEnabled()) {
          setFlat(true);
          setEnabled(false);
      }
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
    if (ret == QMessageBox::Yes) {
        m_deskview->writeSettings();
        exit(0);
    } else
         event->ignore();
}

void FormBid::misere () { _GamesType = g86; accept(); }
void FormBid::pass () { _GamesType = gtPass; accept(); }
void FormBid::slotWhist () { _GamesType = whist; accept(); }
void FormBid::slotHalfWhist () { _GamesType = halfwhist; accept(); }
void FormBid::slotWithoutThree () { _GamesType = withoutThree; accept(); }

void FormBid::score ()
{
  m_deskview->drawPool();
}


void FormBid::onBidClick () {
  QBidButton *b = static_cast<QBidButton *>(sender());
  if (!b) return;
  _GamesType = b->bid();
  accept();
}


void FormBid::enableAll () {
  foreach (QPushButton *b, m_bidButtons) {
      (static_cast<QBidButton *>(b))->enable();
  }
}


void FormBid::disableAll () {
    foreach (QPushButton *b, m_bidButtons)
        (static_cast<QBidButton *>(b))->disable();
    foreach (QPushButton *button, m_otherButtons)
        button->setEnabled(false);
}


void FormBid::disableLessThan (eGameBid gameType) {
  foreach (QPushButton *b, m_bidButtons) {
      if (gameName2Type(b->objectName()) < gameType)
          (static_cast<QBidButton *>(b))->disable();
  }
}


void FormBid::disableItem (eGameBid gameType) {
  QList<QPushButton *> bList = qFindChildren<QPushButton *>(this);
  foreach (QPushButton *b, bList) {
      if (gameName2Type(b->objectName()) == gameType)
          b->setEnabled(false);
  }
}


void FormBid::enableItem (eGameBid gameType) {
  QList<QPushButton *> bList = qFindChildren<QPushButton *>(this);
  foreach (QPushButton *b, bList) {
      if (gameName2Type(b->objectName()) == gameType)
          b->setEnabled(true);
  }
}

void FormBid::disableMisere()
{
    (static_cast<QBidButton *>(btnMisere))->disable();
}

void FormBid::enableWithoutThree()
{
    btnWithoutThree->setEnabled(true);
}

void FormBid::disableWithoutThree()
{
    btnWithoutThree->setEnabled(false);
}

void FormBid::enableScore()
{
    btnShowScore->setEnabled(true);
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
	  m_bidButtons.append(b);
      connect(b, SIGNAL(clicked()), this, SLOT(onBidClick()));
    }
  }

  btnMisere = new QBidButton(g86, 0, 0, this);
  btnMisere->setGeometry(10, 100, 200, 27);
  btnMisere->setMinimumSize(0,0);
  btnMisere->setObjectName("g86");
  btnMisere->setText(tr("&Misere"));
  m_bidButtons.append(btnMisere);
  
  btnPass = new QPushButton(this);
  btnPass->setGeometry(10,190,60,30);
  btnPass->setMinimumSize(0,0);
  btnPass->setObjectName("pass");
  btnPass->setText(tr("&Pass"));
  btnPass->setIcon(QIcon(QString(":/pics/pass.png")));
  m_otherButtons.append(btnPass);

  btnWhist = new QPushButton(this);
  btnWhist->setGeometry(150,190,60,30);
  btnWhist->setMinimumSize(0,0);
  btnWhist->setObjectName("whist");
  btnWhist->setText(tr("&Whist"));
  btnWhist->setIcon(QIcon(QString(":/pics/whist.png")));
  m_otherButtons.append(btnWhist);

  btnHalfWhist = new QPushButton(this);
  btnHalfWhist->setGeometry(70, 190, 80, 30);
  btnHalfWhist->setMinimumSize(0,0);
  btnHalfWhist->setObjectName("halfwhist");
  btnHalfWhist->setText(tr("&HalfWhist"));
  btnHalfWhist->setIcon(QIcon(QString(":/pics/halfwhist.png")));
  m_otherButtons.append(btnHalfWhist);

  btnShowScore = new QPushButton(this);
  btnShowScore->setGeometry(116, 220, 94, 27);
  btnShowScore->setMinimumSize(0, 0);
  btnShowScore->setText(tr("S&core"));
  btnShowScore->setToolTip(tr("Show game table with calculated scores"));
  btnShowScore->setIcon(QIcon(QString(":/pics/paper.png")));
  m_otherButtons.append(btnShowScore);

  btnWithoutThree = new QPushButton(this);
  btnWithoutThree->setGeometry(10, 220, 106, 27);
  btnWithoutThree->setMinimumSize(0, 0);
  btnWithoutThree->setText(tr("Without &Three"));
  btnWithoutThree->setToolTip(tr("Cancel game with penalty of three untaken tricks. No whists are written"));
  btnWithoutThree->setIcon(QIcon(QString(":/pics/cancel.png")));
  m_otherButtons.append(btnWithoutThree);

  connect(btnMisere, SIGNAL(clicked()), this, SLOT(misere()));
  connect(btnPass, SIGNAL(clicked()), this, SLOT(pass()));
  connect(btnWhist, SIGNAL(clicked()), this, SLOT(slotWhist()));
  connect(btnHalfWhist, SIGNAL(clicked()), this, SLOT(slotHalfWhist()));
  connect(btnWithoutThree, SIGNAL(clicked()), this, SLOT(slotWithoutThree()));
  connect(btnShowScore, SIGNAL(clicked()), this, SLOT(score()));

}
