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
