#ifndef CARD_H
#define CARD_H

#include <QList>


const int FACE_JACK  = 11;
const int FACE_QUEEN = 12;
const int FACE_KING  = 13;
const int FACE_ACE   = 14;


class Card {
public:
  Card (int aFace, int aSuit);
  Card (int aPacked);
  ~Card ();

  inline int face () const { return mFace; }
  inline int suit () const { return mSuit; }
  inline bool isValid () const { return mValid; }

  int pack () const;

  friend int operator > (const Card &c0, const Card &c1);
  friend int operator < (const Card &c0, const Card &c1);
  friend int operator >= (const Card &c0, const Card &c1);
  friend int operator <= (const Card &c0, const Card &c1);
  friend int operator == (const Card &c0, const Card &c1);
  friend int operator != (const Card &c0, const Card &c1);

private:
  void validate ();
  int compareWith (const Card &c1) const;

private:
  int mFace; //от 7,8,9,10,11j,12q,13k,14a
  int mSuit; //от 1 до 4
  bool mValid;
};


typedef QList<Card *> QCardList;


#endif
