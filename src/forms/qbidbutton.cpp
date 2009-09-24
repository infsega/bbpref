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

#include "qbidbutton.h"


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


QBidButton::~QBidButton () {
}
