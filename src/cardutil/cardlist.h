//---------------------------------------------------------------------------
#ifndef CARDLIST_H
#define CARDLIST_H

#include "prfconst.h"
#include "card.h"


// doesn't own the cards
class CardList {
public:
  CardList ();
  ~CardList ();

  virtual void clear ();

  void mySort ();
  Card *exists (int aFace, int aSuit) const;
  Card *exists (int aPacked) const;
  Card *exists (const Card &cc) const;

  Card *minInSuit (int aSuit) const;
  Card *maxInSuit (int aSuit) const;
  Card *minCard () const;
  Card *maxCard () const;

  bool hasSuit (int aSuit) const;

  Card *greaterInSuit (int aFace, int aSuit) const; //первая больше чем переданная
  Card *lesserInSuit (int aFace, int aSuit) const; //первая меньше чем переданная
  Card *greaterInSuit (Card *card) const; //первая больше чем переданная
  Card *lesserInSuit (Card *card) const; //первая меньше чем переданная

  void copySuit (const CardList *src, eSuit aSuit); // copy only selected suit

  int cardsInSuit (int aSuit) const;
  int count () const;
  int emptySuit (int aSuit) const; //возврат масти (за исключение данной) в которой нет карт

  inline Card *at (int idx) const {
    if (idx < 0 || idx >= mList.size()) return 0;
    return mList[idx];
  }
  inline void putAt (int idx, Card *c) {
    if (idx < 0) return;
    while (idx >= mList.size()) mList.append(0);
    mList[idx] = c;
  }
  inline void removeAt (int idx) { if (idx >= 0 && idx < mList.size()) mList[idx] = 0; }
  inline void remove (Card *c) { removeAt(mList.indexOf(c)); }
  inline int append (Card *c) {
    int idx = mList.indexOf(0);
    if (idx < 0) {
      idx = mList.size();
      mList << c;
    } else mList[idx] = c;
    return idx;
  }
  inline void free (Card *c) {
    int idx = mList.indexOf(c);
    if (idx >= 0) {
      mList[idx] = 0;
      delete c;
    }
  }
  inline int size () const { return mList.size(); }
  virtual void shallowCopy (CardList *list);

protected:
  void removeNulls ();

protected:
  QCardList mList;
};


#endif
