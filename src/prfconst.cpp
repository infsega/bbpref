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

#include <stdlib.h>
#include "prfconst.h"
#include "card.h"

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
}


int gameTricks (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 7;
  if (gType >= g81 && gType <= 85) return 8;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 9;
  if (gType >= g101 && gType <= 105) return 10;
  return 6;
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
