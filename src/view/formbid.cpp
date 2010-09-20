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

#include <QtGui/QMessageBox>
#include <QtGui/QToolTip>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QDebug>

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
    QBidButton (eGameBid aBid, QWidget *parent=0);
    eGameBid bid () const { return mBid; }
    void enable();
    void disable();
    #ifndef MOBILE
      QSize sizeHint() const { return QSize(BidIconWidth+5, BidIconHeight+5); }
    #endif

  protected:
    eGameBid mBid;
  };

  QBidButton::QBidButton (eGameBid aBid, QWidget *parent) : QPushButton(parent), mBid(aBid)
  {
    QString iName, oName;
    iName.sprintf(":/pics/bids/s%i.png", aBid);
    oName.sprintf("g%i", aBid);
    setObjectName(oName);
    setContentsMargins(2,2,0,0);
    setIconSize(QSize(BidIconWidth, BidIconHeight));
    setIcon(QIcon(iName));
    #ifdef MOBILE
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    #endif
  }

  void QBidButton::enable()
  {
      if(!isEnabled()) {
          setEnabled(true);
          setFlat(false);
      }
  }
  void QBidButton::disable()
  {
      if(isEnabled()) {
          setFlat(true);
          setEnabled(false);
      }
  }
} // end of namespace


BidDialog::BidDialog (DeskView *parent) : QDialog (parent)
{
  //this->setModal(true);
  m_deskview = parent;
  setWindowTitle(tr("Bidding"));
  setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum));
  initDialog();
}


BidDialog::~BidDialog () {
}

BidDialog* BidDialog::instance(DeskView *parent)
{
  static BidDialog *obj = 0;

    if (!obj)
      obj = new BidDialog(parent);

    return obj;
}

void BidDialog::showEvent(QShowEvent *event)
{
  #ifdef MOBILE
    showFullScreen();
  #else
    QDialog::showEvent(event);
  #endif
}

void BidDialog::closeEvent(QCloseEvent *event)
{
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

void BidDialog::misere () { _GamesType = g86; accept(); }
void BidDialog::pass () { _GamesType = gtPass; accept(); }
void BidDialog::slotWhist () { _GamesType = whist; accept(); }
void BidDialog::slotHalfWhist () { _GamesType = halfwhist; accept(); }
void BidDialog::slotWithoutThree () { _GamesType = withoutThree; accept(); }

void BidDialog::score ()
{
  m_deskview->drawPool();
}


void BidDialog::onBidClick ()
{
  QBidButton *b = static_cast<QBidButton *>(sender());
  if (!b) return;
  _GamesType = b->bid();
  accept();
}


void BidDialog::enableBids ()
{
  foreach (QPushButton *b, m_bidButtons)
    static_cast<QBidButton *>(b)->enable();
}


void BidDialog::disableBids ()
{
  foreach (QPushButton *b, m_bidButtons)
    static_cast<QBidButton *>(b)->disable();
}


void BidDialog::disableLessThan (eGameBid gameType)
{
  foreach (QPushButton *b, m_bidButtons) {
    if (gameName2Type(b->objectName()) < gameType)
      static_cast<QBidButton *>(b)->disable();
  }
}


void BidDialog::setActiveButtons(const ActiveButtons buttons)
{
  if (buttons & Whist)
    btnWhist->setEnabled(true);
  else
    btnWhist->setEnabled(false);

  if (buttons & Pass)
    btnPass->setEnabled(true);
  else
    btnPass->setEnabled(false);

  if (buttons & HalfWhist)
    btnHalfWhist->setEnabled(true);
  else
    btnHalfWhist->setEnabled(false);

  if (buttons & WithoutThree)
    btnWithoutThree->setEnabled(true);
  else
    btnWithoutThree->setEnabled(false);

  if (buttons & Misere)
    static_cast<QBidButton *>(btnMisere)->enable();
  else
    static_cast<QBidButton *>(btnMisere)->disable();

  if (buttons & Score)
    btnScore->setEnabled(true);
  else
    btnScore->setEnabled(false);
}


void BidDialog::initDialog ()
{
//  setFixedSize(220, 260);
  QVBoxLayout *dialogLayout = new QVBoxLayout(this);
  QGridLayout *bidLayout = new QGridLayout();
  bidLayout->setSpacing(2);
  dialogLayout->addLayout(bidLayout);
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setSpacing(2);
  dialogLayout->addLayout(buttonLayout);
  QHBoxLayout *buttonLayout2 = new QHBoxLayout();
  buttonLayout2->setSpacing(2);
  dialogLayout->addLayout(buttonLayout2);
  setLayout(dialogLayout);

  eGameBid bid;
  for (int i=0; i<5; i++) { // 6...10
    for (int j=0; j<5; j++) {
      bid = (eGameBid)((i+6)*10+j+1);
      QBidButton *b = new QBidButton(bid, this);
      if (j == 4)
        b->setToolTip(tr("No trumps"));
      if (i < 3) // 6..8
        bidLayout->addWidget(b, i, j);
      else // 9..10
        bidLayout->addWidget(b, i+1, j);
      m_bidButtons.append(b);
      connect(b, SIGNAL(clicked()), this, SLOT(onBidClick()));
    }
  }

  btnMisere = new QBidButton(g86, this);
  btnMisere->setObjectName("g86");
  btnMisere->setText(tr("&Misere"));
  bidLayout->addWidget(btnMisere, 3, 0, 1, -1);
  m_bidButtons.append(btnMisere);

  btnWhist = new QPushButton(this);
  btnWhist->setObjectName("whist");
  btnWhist->setText(tr("&Whist"));
  btnWhist->setIcon(QIcon(":/pics/whist.png"));
  m_otherButtons.append(btnWhist);
  buttonLayout->addWidget(btnWhist);

  btnHalfWhist = new QPushButton(this);
  btnHalfWhist->setObjectName("halfwhist");
  btnHalfWhist->setText(tr("&HalfWhist"));
  btnHalfWhist->setIcon(QIcon(":/pics/halfwhist.png"));
  m_otherButtons.append(btnHalfWhist);
  buttonLayout->addWidget(btnHalfWhist);

  btnPass = new QPushButton(this);
  btnPass->setObjectName("pass");
  btnPass->setText(tr("&Pass"));
  btnPass->setIcon(QIcon(":/pics/pass.png"));
  m_otherButtons.append(btnPass);
  buttonLayout->addWidget(btnPass);

  btnScore = new QPushButton(this);
  btnScore->setText(tr("S&core"));
  btnScore->setToolTip(tr("Show game table with calculated scores"));
  btnScore->setIcon(QIcon(":/pics/paper.png"));
  m_otherButtons.append(btnScore);
  buttonLayout2->addWidget(btnScore);

  btnWithoutThree = new QPushButton(this);
  btnWithoutThree->setText(tr("Without &Three"));
  btnWithoutThree->setToolTip(tr("Cancel game with penalty of three untaken tricks. No whists are written"));
  btnWithoutThree->setIcon(QIcon(":/pics/cancel.png"));
  m_otherButtons.append(btnWithoutThree);
  buttonLayout2->addWidget(btnWithoutThree);

  connect(btnMisere, SIGNAL(clicked()), this, SLOT(misere()));
  connect(btnPass, SIGNAL(clicked()), this, SLOT(pass()));
  connect(btnWhist, SIGNAL(clicked()), this, SLOT(slotWhist()));
  connect(btnHalfWhist, SIGNAL(clicked()), this, SLOT(slotHalfWhist()));
  connect(btnWithoutThree, SIGNAL(clicked()), this, SLOT(slotWithoutThree()));
  connect(btnScore, SIGNAL(clicked()), this, SLOT(score()));

}
