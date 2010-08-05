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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "prfconst.h"
#include "card.h"

bool optStalingrad = true;
bool opt10Whist = false;
bool optWhistGreedy = true;
int optMaxPool = 10;
bool optQuitAfterMaxRounds = false;
int optMaxRounds = -1;

#if defined Q_WS_X11 || defined Q_WS_QWS || defined Q_WS_MAC
	QString optHumanName = getenv("USER");
#elif defined Q_WS_WIN
  QString optHumanName = ""; // get user name from WinAPI
#else
	QString optHumanName = "";
#endif
QString optPlayerName1 = "Player 1";
bool optAlphaBeta1 = false;
QString optPlayerName2 = "Player 2";
bool optAlphaBeta2 = false;
bool optDebugHands = false;
bool optAggPass = false;
int optPassCount = 0;
bool optWithoutThree = true;

bool optDealAnim = true;
bool optTakeAnim = true;
bool optPrefClub = false;

int CARDWIDTH = 71;
int CARDHEIGHT = 96;

int succBid (eGameBid game) {
  switch (game) {
    case g86: return g91; // after misere
    case g105: return g105; // last
    case gtPass: return g61; // first
    default: ;
  }
  if (game != undefined)  {
    int vz = game/10, mast = game-vz*10;
    if (mast != 5) {
      mast++;
    } else {
      mast = SuitSpades;
      vz++;
    }
    return vz*10+mast;
  }
  return g61;
  //return gtPass;
}


int gameTricks (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 7;
  if (gType >= g81 && gType <= 85) return 8;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 9;
  if (gType >= g101 && gType <= 105) return 10;
  return 6;
}


int gameWhists (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 2;
  if (gType >= g81 && gType <= 85) return 1;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 1;
  if (gType >= g101 && gType <= 105) {
	  if (opt10Whist)
	  	return 1;
	  else
	  	return 0;
  }
  return 4;
}


int gameWhistsMin (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 2;
  if (gType >= g81 && gType <= 85) return 1;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 1;
  if (gType >= g101 && gType <= 105) return 0;
  return 3;
}


int gamePoolPrice (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 4;
  if (gType >= g81 && gType <= 85) return 6;
  if (gType == g86) return 10;
  if (gType >= g91 && gType <= 95) return 8;
  if (gType >= g101 && gType <= 105) return 10;
  if (gType == raspass) return 1;
  return 2;
}
