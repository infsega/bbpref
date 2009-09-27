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

#include <stdlib.h>
#include <stdio.h>

#include <QPixmap>
#include <QPainter>

#include "player.h"


Player::Player (int aMyNumber, DeskView *aDeskView) : mDeskView(aDeskView), mIStart(false), mPlayerNo(aMyNumber) {
  internalInit();
}


Player::Player (const Player &pl) {
  internalInit();
  clone(&pl);
}


Player::~Player () {
  clear();
}


inline void Player::internalInit () {
  clear();
}


Player &Player::operator = (const Player &pl) {
  clone(&pl);
  return *this;
}


void Player::clone (const Player *pl) {
  if (pl && pl != this) {
    mDeskView = pl->mDeskView;
    mCards = pl->mCards;
    mLeft = pl->mLeft;
    mRight = pl->mRight;
    mOut = pl->mOut;
    mCardsOut = pl->mCardsOut;
    mCardCarryThru = pl->mCardCarryThru;
    mNick = pl->mNick;
    mMessage = pl->mMessage;
    mPlayerNo = pl->mPlayerNo;
    mInvisibleHand = pl->mInvisibleHand;
    mMyGame = pl->mMyGame;
    mTricksTaken = pl->mTricksTaken;
    mPrevHiCardIdx = pl->mPrevHiCardIdx;
  }
}


void Player::clear () {
  mCards.clear();
  mLeft.clear();
  mRight.clear();
  mOut.clear();
  mCardsOut.clear();
  mCardCarryThru = 0;
  mMessage.clear();
  mInvisibleHand = (mPlayerNo != 1);
  mMyGame = undefined;
  mTricksTaken = 0;
  mWaitingForClick = false;
  mPrevHiCardIdx = -1;
}


inline bool Player::invisibleHand () const {
  return optDebugHands ? false : mInvisibleHand;
}


inline void Player::setInvisibleHand (bool invis) {
  mInvisibleHand = invis;
}

inline void Player::setCurrentStart (bool start) {
	mIStart = start;
}


inline void Player::sortCards () {
  mCards.mySort();
}


inline void Player::dealCard (Card *aCard) {
  mCards.insert(aCard);
}


void Player::returnDrop () {
  if (mCardsOut.at(0)) mCards.insert(mCardsOut.at(0));
  if (mCardsOut.at(1)) mCards.insert(mCardsOut.at(1));
  mCardsOut.clear();
}


inline void Player::gotTrick () {
  mTricksTaken++;
}


inline void Player::gotPassPassTricks (int cnt) {
  mTricksTaken = cnt;
}


///////////////////////////////////////////////////////////////////////////////
// game graphics
///////////////////////////////////////////////////////////////////////////////

void Player::getLeftTop (int *left, int *top) {
  *left = 0; *top = 0;
  if (!mDeskView) return;
  switch (mPlayerNo) {
    case 1:
      *left = (mDeskView->DesktopWidth-(mDeskView->DesktopWidth/2-2*mDeskView->xBorder))/2;
      *top = mDeskView->DesktopHeight-(mDeskView->yBorder)-CARDHEIGHT;//mDeskView->CardHeight;//-10;
      break;
    case 2:
      *left = mDeskView->xBorder;
      *top = mDeskView->yBorder+20;
      break;
    case 3:
      *left = mDeskView->DesktopWidth-mDeskView->xBorder;
      *top = mDeskView->yBorder+20;
      break;
    default: ;
  }
}


///////////////////////////////////////////////////////////////////////////////
// build array with cards offsets and indicies
// at least 28 ints (14 int pairs); return # of used ints; the last int is -1
// result: ofs, cardNo, ..., -1
int Player::buildHandXOfs (int *dest, int startX, bool opened) {
  int cnt = 0, oldXX = startX, *oDest = dest;
  Card *cur = 0, *prev = 0;

  CardList lst(mCards);
  lst.mySort();

  if (mPlayerNo == 3) startX = 0;
  // normal
  startX -= opened ? SUIT_OFFSET : CLOSED_CARD_OFFSET ;
  for (int f = 0; f < lst.size(); f++) {
    Card *pp = lst.at(f);
    if (!pp) continue;
    prev = cur;
    cur = pp;
    if (opened) {
      startX += (prev && prev->suit() != cur->suit()) ? NEW_SUIT_OFFSET : SUIT_OFFSET ;
    } else startX += CLOSED_CARD_OFFSET;
    *dest++ = startX;
    int idx = mCards.indexOf(cur);
    *dest++ = idx;
    if (idx < 0) abort();
    cnt++;
    if (cnt > 12) abort();
  }
  *dest++ = -1;
  *dest = -1;

  if (mPlayerNo == 3 && cnt) {
    // righttop
    startX = oldXX-(oDest[(cnt-1)*2]+CARDWIDTH+4);
    for (int f = 0; f < cnt; f++) oDest[f*2] += startX;
  }

  return cnt;
}


int Player::cardAt (int lx, int ly, bool opened) {
  int res = -1, ofs[28];
  int left, top;
  getLeftTop(&left, &top);
  buildHandXOfs(ofs, left, opened);
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x1 = ofs[f], y1 = top;//+mDeskView->yBorder;
    int x2 = x1+CARDWIDTH, y2 = y1+CARDHEIGHT;
    if (x1 < lx && lx < x2 && y1 < ly && ly < y2) res = ofs[f+1];
  }
  return res;
}


void Player::drawAt (DeskView *aDeskView, int left, int top, int selNo) {
  int ofs[28];
  DeskView *oDesk = mDeskView;

  if (!aDeskView) return;
  mDeskView = aDeskView;

  int cnt = buildHandXOfs(ofs, left, !invisibleHand());
  if (cnt) left = ofs[0];
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x = ofs[f], y = top;
    Card *card = mCards.at(ofs[f+1]);
    aDeskView->drawCard(card, x, y, !invisibleHand(), ofs[f+1]==selNo);
  }
  aDeskView->emitRepaint();
  mDeskView = oDesk;
}


void Player::draw () {
  int left, top;
  if (!mDeskView) return;
  getLeftTop(&left, &top);
  drawAt(mDeskView, left, top, mPrevHiCardIdx);
}


void Player::clearCardArea () {
  int left, top, ofs[28];
  if (!mDeskView) return;
  getLeftTop(&left, &top);
  int cnt = buildHandXOfs(ofs, left, !invisibleHand());
  if (!cnt) return;
  for (int f = 0; ofs[f] >= 0; f += 2) {
    int x = ofs[f], y = top;
    mDeskView->ClearBox(x, y, CARDWIDTH, CARDHEIGHT);
  }
}


void Player::hilightCard (int lx, int ly) {
  Q_UNUSED(lx)
  Q_UNUSED(ly)
  mPrevHiCardIdx = -1;
}
