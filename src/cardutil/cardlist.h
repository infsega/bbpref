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

//---------------------------------------------------------------------------
#ifndef CARDLIST_H
#define CARDLIST_H

#include <QByteArray>

#include "prfconst.h"
#include "card.h"


// doesn't own the cards
class CardList {
public:
  CardList ();
  CardList (const CardList &cl);
  ~CardList ();

  void clear ();

  void mySort ();
  Card *exists (int aFace, int aSuit) const;
  Card *exists (Card *cc) const;

  Card *minInSuit (int aSuit) const;
  Card *maxInSuit (int aSuit) const;
  Card *minFace () const;
  Card *maxFace () const;

  bool hasSuit (int aSuit) const;

  Card *greaterInSuit (int aFace, int aSuit) const; //первая больше чем переданная
  Card *lesserInSuit (int aFace, int aSuit) const; //первая меньше чем переданная
  Card *greaterInSuit (Card *card) const; //первая больше чем переданная
  Card *lesserInSuit (Card *card) const; //первая меньше чем переданная

  void copySuit (const CardList *src, eSuit aSuit); // copy only selected suit

  int cardsInSuit (int aSuit) const;
  int count () const;
  int emptySuit (int aSuit) const; //возврат масти (за исключение данной) в которой нет карт

  inline int indexOf (Card *cc) const {
    if (!cc) return -1;
    return mList.indexOf(cc);
  }
  inline Card *at (int idx) const {
    if (idx < 0 || idx >= mList.size()) return 0;
    return mList[idx];
  }
  inline void putAt (int idx, Card *c) {
    if (idx < 0) return;
    while (idx >= mList.size()) mList << 0;
    mList[idx] = c;
  }
  inline void removeAt (int idx) { if (idx >= 0 && idx < mList.size()) mList[idx] = 0; }
  inline void remove (Card *c) { removeAt(mList.indexOf(c)); }
  inline int insert (Card *c) {
    int idx = mList.indexOf(0);
    if (idx < 0) {
      idx = mList.size();
      mList << c;
    } else mList[idx] = c;
    return idx;
  }

  inline int size () const { return mList.size(); }

  void serialize (QByteArray &ba) const;
  bool unserialize (QByteArray &ba, int *pos);

  CardList &operator = (const CardList &cl);
  CardList &operator = (const CardList *cl);
  CardList &operator << (Card *cc);

  void newDeck ();
  void shuffle ();

protected:
  //void removeNulls ();
  void shallowCopy (const CardList *list);
  void shallowCopy (const CardList &list);

protected:
  QCardList mList;
};


#endif
