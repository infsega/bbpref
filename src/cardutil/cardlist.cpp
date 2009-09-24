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

#include "cardlist.h"

#include "baser.h"


CardList::CardList () {
}


CardList::CardList (const CardList &cl) {
  shallowCopy(cl);
}


CardList::~CardList () {
  clear();
}


void CardList::clear () {
  mList.clear();
}


Card *CardList::exists (int aFace, int aSuit) const {
  Card *c = newCard(aFace, aSuit);
  return exists(c);
}


// can compare pointers, 'cause cards are singletones
Card *CardList::exists (Card *cc) const {
  if (!cc) return 0;
  if (mList.indexOf(cc) >= 0) return cc;
  return 0;
}


Card *CardList::minFace () const {
  return minInSuit(-1);
}


Card *CardList::maxFace () const {
  return maxInSuit(-1);
}


Card *CardList::minInSuit (int aSuit) const {
  Card *res = 0;
  foreach (Card *c, mList) {
    if (!c) continue;
    if (aSuit > 0 && c->suit() != aSuit) continue;
    if (!res || res->face() > c->face()) res = c;
  }
  return res;
}


Card * CardList::maxInSuit (int aSuit) const {
  Card *res = 0;
  foreach (Card *c, mList) {
    if (!c) continue;
    if (aSuit > 0 && c->suit() != aSuit) continue;
    if (!res || res->face() < c->face()) res = c;
  }
  return res;
}


int CardList::cardsInSuit (int aSuit) const {
  int res = 0;
  foreach (Card *c, mList) if (c && c->suit() == aSuit) res++;
  return res;
}


int CardList::count () const {
  int res = 0;
  foreach (Card *c, mList) if (c) res++;
  return res;
}


void CardList::mySort () {
  int cnt = mList.size();
  // bubble sort
  for (int f = 0; f < cnt; f++) {
    for (int c = cnt-1; c > f; c--) {
      Card *cf = mList[f], *cc = mList[c];
      if (cf || cc) {
        int r = 0;
        if (cf && cc) {
          r = (cf->suit())-(cc->suit());
          if (!r) r = (cc->face())-(cf->face());
        } else if (!cf) r = 1; // so nulls will go down
        if (r > 0) mList.swap(f, c);
      } else {
        // both empty
      }
    }
  }
}


Card *CardList::greaterInSuit (int aFace, int aSuit) const {
  if (aFace > FACE_ACE) return 0;
  if (aFace < 7) aFace = 7;
  while (aFace <= FACE_ACE) {
    Card *found = exists(aFace, aSuit);
    if (found) return found;
    aFace++;
  }
  return 0;
}


Card *CardList::greaterInSuit (Card *card) const {
  if (!card) return 0;
  return greaterInSuit(card->face(), card->suit());
}


Card *CardList::lesserInSuit (int aFace, int aSuit) const {
  if (aFace < 7) return 0;
  if (aFace > FACE_ACE) aFace = FACE_ACE;
  while (aFace >= 7) {
    Card *found = exists(aFace, aSuit);
    if (found) return found;
    aFace--;
  }
  return 0;
}


//первая меньше чем переданная
Card *CardList::lesserInSuit (Card *card) const {
  if (!card) return 0;
  return lesserInSuit(card->face(), card->suit());
}


bool CardList::hasSuit (int aSuit) const {
  foreach (Card *c, mList) {
    if (c && c->suit() == aSuit) return true;
  }
  return false;
}


int CardList::emptySuit (int aSuit) const {
  for (int f = 1; f <= 4; f++) {
    if (f == aSuit) continue; //k8:bug? break;
    if (!hasSuit(f)) return f;
  }
  return 0;
}


void CardList::copySuit (const CardList *src, eSuit aSuit) {
  // remove existing cards
/*
  QMutableListIterator<Card *> i(mList);
  while (i.hasNext()) {
    Card *c = i.next();
    if (c->suit() == (int)aSuit) i.remove();
  }
*/
  for (int f = mList.size()-1; f >= 0; f--) {
    Card *c = mList[f];
    if (c && c->suit() == aSuit) mList[f] = 0;
  }
  // copy cards
  foreach (Card *c, src->mList) {
    if (c && c->suit() == (int)aSuit) insert(c);
  }
}


void CardList::shallowCopy (const CardList *list) {
  clear();
  if (!list) return;
  foreach (Card *c, list->mList) mList << c;
}


void CardList::shallowCopy (const CardList &list) {
  clear();
  foreach (Card *c, list.mList) mList << c;
}


void CardList::serialize (QByteArray &ba) const {
  serializeInt(ba, mList.size());
  for (int f = 0; f < mList.size(); f++) {
    Card *c = mList[f];
    int i = 0;
    if (c) i = (c->face()-7)*10+c->suit();
    serializeInt(ba, i);
  }
}


bool CardList::unserialize (QByteArray &ba, int *pos) {
  clear();
  int cnt;
  if (!unserializeInt(ba, pos, &cnt)) return false;
  while (cnt-- > 0) {
    int t;
    if (!unserializeInt(ba, pos, &t)) return false;
    if (t < 1 || t > 74) t = 0;
    if (t) {
      int face = (t/10)+7, suit = t%10;
      if (face >= 7 && face <= FACE_ACE && suit >= 1 && suit <= 4) mList << newCard(face, suit);
      else mList << 0;
    } else mList << 0;
  }
  return true;
}


CardList &CardList::operator = (const CardList &cl) {
  if (&cl != this) shallowCopy(cl);
  return *this;
}


CardList &CardList::operator = (const CardList *cl) {
  if (cl != this) shallowCopy(cl);
  return *this;
}


CardList &CardList::operator << (Card *cc) {
  if (cc) insert(cc);
  return *this;
}


void CardList::newDeck () {
  mList.clear();
  for (int suit = 1; suit <= 4; suit++) {
    for (int face = 7; face <= FACE_ACE; face++) {
      mList << newCard(face, suit);
    }
  }
}


void CardList::shuffle () {
  // remove nulls
  QMutableListIterator<Card *> i(mList);
  while (i.hasNext()) {
    Card *c = i.next();
    if (!c) i.remove();
  }
  // Fisher-Yates shuffler
  for (int f = mList.size()-1; f >= 0; f--) {
    int n = (qrand()/256)%(f+1); // 0<=n<=f
    mList.swap(f, n);
  }
}
