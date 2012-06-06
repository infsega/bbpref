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

#ifndef PRFCONST_H
#define PRFCONST_H

#define BBPREF_VERSION "1.0"

enum eGameBid {
  zerogame=0,
  showpool=1,
  g86catch=56,
  raspass=57,
  whist=58,
  halfwhist=59,
  undefined=55,
  withoutThree=54,
  gtPass=60,
  g61=61,g62,g63,g64,g65,
  g71=71,g72,g73,g74,g75,
  g81=81,g82,g83,g84,g85,g86,
  g91=91,g92,g93,g94,g95,
  g101=101,g102,g103,g104,g105
};// g86 - miser


int succBid (eGameBid game);
int gameTricks (eGameBid gType);
int gameWhistsMin (eGameBid gType);
int gamePoolPrice (eGameBid gType);

#endif
