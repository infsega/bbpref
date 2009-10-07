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

#include <limits.h>
#include "ncounter.h"


WrapCounter::WrapCounter () {
  nMin = nValue = 0;
  nMax = INT_MAX;
}


WrapCounter::WrapCounter (int _nMin, int _nMax) {
  nMin = nValue = _nMin;
  nMax = _nMax;
}


WrapCounter::WrapCounter (int _nValue, int _nMin, int _nMax) {
  nMin = _nMin;
  nMax = _nMax;
  nValue = (_nValue-nMin)%(nMax-nMin+1)+nMin;
}


WrapCounter &operator - (WrapCounter &c, int n) {
  while (n-- > 0) --c;
  return c;
}


WrapCounter &operator + (WrapCounter &c,int n) {
  while (n-- > 0) ++c;
  return c;
}


int succPlayer (const WrapCounter &aCurrent) {
  int res = aCurrent.nValue+1;
  if (res > aCurrent.nMax) res = aCurrent.nMin;
  return res;
}

int predPlayer(const WrapCounter &aCurrent) {
  int res = aCurrent.nValue-1;
  if (res < aCurrent.nMin) res = aCurrent.nMax;
  return res;
}
