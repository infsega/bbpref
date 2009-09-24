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

#include "card.h"


Card::Card (int aFace, int aSuit) {
  mFace = aFace;
  mSuit = aSuit;
  validate();
}


Card::~Card () {
}


void Card::validate () {
  mValid = (mFace >= 7 && mFace <= 14 && mSuit >= 1 && mSuit <= 4);
}


int Card::pack () const {
  if (!mValid) return 0;
  return mFace*10+mSuit;
}


int Card::compareWith (const Card &c1) const {
  int r = mSuit-c1.mSuit;
  if (!r) r = mFace-c1.mFace;
  return r;
}


QString Card::toString () const {
  QString res("???");
  if (!mValid) return res;
  switch (mFace) {
    case 7: res = " 7"; break;
    case 8: res = " 8"; break;
    case 9: res = " 9"; break;
    case 10: res = "10"; break;
    case 11: res = " J"; break;
    case 12: res = " Q"; break;
    case 13: res = " K"; break;
    case 14: res = " A"; break;
    default: return res;
  }
  switch (mSuit) {
    case 1: res += "s"; break;
    case 2: res += "c"; break;
    case 3: res += "d"; break;
    case 4: res += "h"; break;
    default: res = "???";
  }
  return res;
}


///////////////////////////////////////////////////////////////////////////////
int operator > (const Card &c0, const Card &c1) {
  return (c0.compareWith(c1) > 0) ? 1 : 0;
}


int operator < (const Card &c0, const Card &c1) {
  return (c0.compareWith(c1) < 0) ? 1 : 0;
}


int operator >= (const Card &c0, const Card &c1) {
  return (c0.compareWith(c1) >= 0) ? 1 : 0;
}


int operator <= (const Card &c0, const Card &c1) {
  return (c0.compareWith(c1) <= 0) ? 1 : 0;
}


int operator == (const Card &c0, const Card &c1) {
  return (c0.compareWith(c1) == 0) ? 1 : 0;
}


int operator != (const Card &c0, const Card &c1) {
  return (c0.compareWith(c1) != 0) ? 1 : 0;
}


///////////////////////////////////////////////////////////////////////////////
static Card *cList[33];
static bool inited = false;


void initCardList () {
  if (inited) return;
  for (int f = 0; f < 32; f++) cList[f] = new Card(f/4+7, f%4+1);
/*
  cList[32] = new Card(7, 1);
  cList[32]->mFace = 0;
  cList[32]->mSuit = 0;
  cList[32]->mValid = false;
*/
  cList[32] = 0;
  inited = true;
}


Card *newCard (int aFace, int aSuit) {
  initCardList();
  int no = 32;
  if (aFace >= 7 && aFace <= FACE_ACE && aSuit >= 1 && aSuit <= 4) no = (aFace-7)*4+(aSuit-1);
  return cList[no];
}


Card *cardFromName (const char *str) {
  int face = -1, suit = -1;
  if (!str) return 0;
  while (*str && (unsigned char)(*str) <= ' ') str++;
  switch (*str++) {
    case '7': face = 7; break;
    case '8': face = 8; break;
    case '9': face = 9; break;
    case '1':
      if (*str++ != '0') return 0;
      face = 10;
      break;
    case 'J': case 'j': face = 11; break;
    case 'Q': case 'q': face = 12; break;
    case 'K': case 'k': face = 13; break;
    case 'A': case 'a': face = 14; break;
    default: return 0;
  }
  while (*str && (unsigned char)(*str) <= ' ') str++;
  switch (*str++) {
    case 'S': case 's': suit = 1; break;
    case 'C': case 'c': suit = 2; break;
    case 'D': case 'd': suit = 3; break;
    case 'H': case 'h': suit = 4; break;
    default: return 0;
  }
  return newCard(face, suit);
}
