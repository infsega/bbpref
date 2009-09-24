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

#ifndef NCOUNTER_H
#define NCOUNTER_H


class WrapCounter {
public:
  WrapCounter ();
  WrapCounter (int _nMin, int _nMax );
  WrapCounter (int _nValue, int _nMin, int _nMax);

  int operator ++ () {
    if (++nValue > nMax) nValue = nMin;
    return nValue;
  }

  int operator -- () {
     if (--nValue < nMin) nValue = nMax;
     return nValue;
  }

  friend WrapCounter &operator - (WrapCounter &c, int n);
  friend WrapCounter &operator + (WrapCounter &c, int n);

public:
  int nValue;
  int nMin;
  int nMax;
};


int succPlayer (const WrapCounter &aCurrent);
int predPlayer (const WrapCounter &aCurrent);


#endif
