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

#ifndef FORMBID_H
#define FORMBID_H

#include <QCloseEvent>
#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QWidget>

#include "prfconst.h"

class DeskView;

/**
 * @class BidDialog formbid.h
 * @brief Helper dialog for bidding
 *
 * Provides a dialog for getting bidding-related actions from human player
 * Set of available choices may be configured using enableBids, disableBids,
 * disableLessThan, and setActiveButtons
 */
class BidDialog : public QDialog  {
 Q_OBJECT

public:
  enum ActiveButtonFlag {
    NoButtons = 0x0,
    Whist = 0x1,
    Pass = 0x2,
    HalfWhist = 0x4,
    WithoutThree = 0x8,
    Misere = 0x10,
    Score = 0x20
  };
  Q_DECLARE_FLAGS(ActiveButtons, ActiveButtonFlag)

  static BidDialog* instance(DeskView *parent=0);

  void enableBids ();
  void disableBids ();
  void disableLessThan (eGameBid);
  void setActiveButtons(const ActiveButtons buttons);

private slots:
  void onBidClick (); // universal handler
  void misere ();
  void pass ();
  void slotWhist ();
  void slotHalfWhist ();
  void slotWithoutThree ();
  void score ();

protected:
  explicit BidDialog (DeskView *parent=0);
  ~BidDialog ();
  void initDialog ();
  void closeEvent(QCloseEvent *event);

public:
  eGameBid _GamesType;
  
private:
  QList<QPushButton *> m_bidButtons;
  QList<QPushButton *> m_otherButtons;
  QPushButton *btnMisere;
  QPushButton *btnPass;
  QPushButton *btnWhist;
  QPushButton *btnWithoutThree;
  QPushButton *btnScore;
  QPushButton *btnHalfWhist;
  DeskView *m_deskview;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(BidDialog::ActiveButtons)

#endif
