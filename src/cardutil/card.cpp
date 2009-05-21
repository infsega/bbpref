#include "card.h"


Card::Card (int aFace, int aSuit) {
  mFace = aFace;
  mSuit = aSuit;
  validate();
}


Card::Card (int aPacked) {
  if (aPacked < 61) {
    mFace = mSuit = 0;
    mValid = false;
  } else {
    mFace = aPacked/10;
    mSuit = aPacked%10;
    validate();
  }
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
