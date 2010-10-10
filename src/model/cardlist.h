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

//---------------------------------------------------------------------------
#ifndef CARDLIST_H
#define CARDLIST_H

#include <QByteArray>

#include "prfconst.h"
#include "card.h"

/**
 * @class CardList cardlist.h
 * @brief Wrapper for QList<Card>
 *
 * This class provides lists of cards with set of sonvenience functions
 * List doesn't own Card pointers, they will not be deleted on list destruction
 */
class CardList {
public:
  CardList ();
  CardList (const CardList &cl);
  ~CardList ();

  void clear ();

  void mySort ();
  Card *exists (int aFace, int aSuit) const;
  Card *exists (Card *cc) const;

  /// Returns card with minimal face and given suit @a aSuit
  Card *minInSuit (int aSuit) const;
  /// @overload
  /// Returns card with minimal face and suit of card @a c
  Card *minInSuit (Card *c) const { Q_ASSERT(c != 0); return minInSuit(c->suit()); }
  /// Returns card with maximal face and given suit @a aSuit
  Card *maxInSuit (int aSuit) const;
  /// @overload
  /// Returns card with maximal face and suit of card @a c
  Card *maxInSuit (Card *c) const { Q_ASSERT(c != 0); return maxInSuit(c->suit()); }
  /// Returns card with minimal face
  Card *minFace () const;
  /// Returns card with maximal face
  Card *maxFace () const;

  /// Returns true if card with suit @a aSuit is present
  bool hasSuit (int aSuit) const;
  /// @overload
  /// Returns true if card with suit of card @a c is present
  bool hasSuit (Card *c) const { Q_ASSERT(c != 0); return hasSuit(c->suit()); }

  /// Returns first card bigger than defined by aFace and aSuit
  Card *greaterInSuit (int aFace, int aSuit) const; //первая больше чем переданная
  Card *lesserInSuit (int aFace, int aSuit) const; //первая меньше чем переданная
  Card *greaterInSuit (Card *card) const; //первая больше чем переданная
  Card *lesserInSuit (Card *card) const; //первая меньше чем переданная

  void copySuit (const CardList *src, eSuit aSuit); // copy only selected suit

  int cardsInSuit (int aSuit) const;
  int count () const;
  int emptySuit (int aSuit) const; //возврат масти (за исключение данной) в которой нет карт

  int indexOf (Card *cc) const {
    if (!cc) return -1;
    return mList.indexOf(cc);
  }
  Card *at (int idx) const {
    if (idx < 0 || idx >= mList.size()) return 0;
    return mList[idx];
  }
  void putAt (int idx, Card *c) {
    if (idx < 0) return;
    while (idx >= mList.size()) mList << 0;
    mList[idx] = c;
  }
  void removeAt (int idx) { if (idx >= 0 && idx < mList.size()) mList[idx] = 0; }
  void remove (Card *c) { removeAt(mList.indexOf(c)); }
  int insert (Card *c) {
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
