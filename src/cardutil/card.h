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

#ifndef CARD_H
#define CARD_H

#include <QList>
#include <QString>


const int FACE_JACK  = 11;
const int FACE_QUEEN = 12;
const int FACE_KING  = 13;
const int FACE_ACE   = 14;


class Card {
public:
  inline int face () const { return mFace; }
  inline int suit () const { return mSuit; }
  inline bool isValid () const { return mValid; }

  int pack () const;

  QString toString () const;

  friend int operator > (const Card &c0, const Card &c1);
  friend int operator < (const Card &c0, const Card &c1);
  friend int operator >= (const Card &c0, const Card &c1);
  friend int operator <= (const Card &c0, const Card &c1);
  friend int operator == (const Card &c0, const Card &c1);
  friend int operator != (const Card &c0, const Card &c1);

  friend Card *newCard (int aFace, int aSuit);
  friend void initCardList ();

private:
  void validate ();
  int compareWith (const Card &c1) const;

  Card (int aFace, int aSuit);
  ~Card ();

private:
  int mFace; //от 7,8,9,10,11j,12q,13k,14a
  int mSuit; //от 1 до 4
  bool mValid;
};


typedef QList<Card *> QCardList;


void initCardList ();
Card *newCard (int aFace, int aSuit);
Card *newCard (int aPacked);

Card *cardFromName (const char *str);


#endif
