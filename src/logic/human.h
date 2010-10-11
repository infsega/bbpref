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

#ifndef HUMAN_H
#define HUMAN_H

#include "player.h"
#include "desktop.h"
#include "deskview.h"

/**
 * @class HumanPlayer human.h
 * @brief Represents player managed by GUI user
 */
class HumanPlayer : public Player {
public:
  HumanPlayer(int aMyNumber, PrefModel *model);

  virtual const QString type() const { return QLatin1String("Human"); }

  virtual Player * create(int aMyNumber, PrefModel *model);
  virtual bool isHuman() const { return true; }

  //HumanPlayer &operator = (const Player &pl);
  //HumanPlayer &operator = (const HumanPlayer &pl);

  virtual void clear ();

  virtual Card *makeMove (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut=false); //ход
  virtual eGameBid makeBid (eGameBid lMove, eGameBid rMove); //ход при торговле
  virtual eGameBid makeFinalBid (eGameBid MaxGame, int nPlayerPass); // после получения игроком прикупа - пасс или вист
  virtual eGameBid makeDrop();

  virtual bool chooseClosedWhist ();

  virtual void highlightCard (int lx, int ly);
};


#endif
