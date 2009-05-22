#include "cardlist.h"


CardList::CardList () {
}


CardList::~CardList () {
  clear();
}


void CardList::clear () {
  mList.clear();
}


void CardList::removeNulls () {
  // remove null items
  QMutableListIterator<Card *> i(mList);
  while (i.hasNext()) {
    Card *c = i.next();
    if (!c) i.remove();
  }
}


Card *CardList::exists (int aPacked) const {
  Card c(aPacked);
  return exists(c);
}


Card *CardList::exists (int aFace, int aSuit) const {
  Card c(aFace, aSuit);
  return exists(c);
}


Card *CardList::exists (const Card &cc) const {
  foreach (Card *c, mList) {
    if (!c) continue;
    if (cc == *c) return c;
  }
  return 0;
}


Card *CardList::minCard () const {
  return minInSuit(-1);
}


Card *CardList::maxCard () const {
  return maxInSuit(-1);
}


Card *CardList::minInSuit (int aSuit) const {
  Card *res = 0;
  foreach (Card *c, mList) {
    if (!c) continue;
    if (aSuit > 0 && c->suit() != aSuit) continue;
    if (!res || res > c) res = c;
  }
  return res;
}


Card * CardList::maxInSuit (int aSuit) const {
  Card *res = 0;
  foreach (Card *c, mList) {
    if (!c) continue;
    if (aSuit > 0 && c->suit() != aSuit) continue;
    if (!res || res < c) res = c;
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
  //removeNulls();
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
  if (aFace >= FACE_ACE) return 0;
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
  if (aFace <= 7) return 0;
  if (aFace > FACE_ACE) aFace = FACE_ACE;
  while (aFace <= 7) {
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
    if (!hasSuit(aSuit)) return f;
  }
  return 0;
}


void CardList::copySuit (const CardList *src, eSuit aSuit) {
  // remove existing cards
  QMutableListIterator<Card *> i(mList);
  while (i.hasNext()) {
    Card *c = i.next();
    if (c->suit() == (int)aSuit) i.remove();
  }
  // copy cards
  foreach (Card *c, src->mList) {
    if (c && c->suit() == (int)aSuit) mList << c;
  }
/*
  Card *card;
  for ( int f = 0;f < _oldlist -> aLimit;f++ )  {
    card =(Card* ) _oldlist ->At(f);
    if ( card && card -> suit()==Mast)  {
        append(card);
    }
  }
*/
}


void CardList::shallowCopy (CardList *list) {
  clear();
  foreach (Card *c, list->mList) mList << c;
}
