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

#include "player.h"
#include "deskview.h"
#include "desktop.h"

#include <QDebug>

#define CARD_WIDTH          (mDeskView->CardWidth)
#define CARD_HEIGHT         (mDeskView->CardHeight)

const int CARD_OFFSET = 22;
#define CARD_OFFSET_P1      (CARD_WIDTH - 5)
#define SUIT_OFFSET         (CARD_WIDTH + 8)
#define SUIT_OFFSET_P1      (CARD_WIDTH + 20)
#define CLOSED_CARD_OFFSET  (CARD_WIDTH * 0.55)

Player::Player (int number, PrefModel *model)
: mScore(model)
, mDeskView(model->view())
, m_model(model)
, mIStart(false)
, mPlayerNo(number)
{
  internalInit();
}


Player::~Player () {
  clear();
}


inline void Player::internalInit () {
  clear();
}


void Player::clear () {
  mCards.clear();
  mCardsOut.clear();
  /// @todo to be moved elsewhere
  mCardCarryThru = 0;
  mMessage.clear();
  /// @todo should be dispatched by model
  mInvisibleHand = (mPlayerNo != 1);
  m_game = undefined;
  /// @todo should be dispatched by model or score board
  mTricksTaken = 0;
  /// @todo should be dispatched by delegate
  mWaitingForClick = false;
  mPrevHiCardIdx = -1;
}


inline bool Player::invisibleHand () const {
  /// @todo should be dispatched by model
  return mDeskView->optDebugHands ? false : mInvisibleHand;
}


inline void Player::setInvisibleHand (bool invis) {
  mInvisibleHand = invis;
}

inline void Player::setCurrentStart (bool start) {
	mIStart = start;
}


void Player::returnDrop () {
  if (mCardsOut.at(0)) mCards.insert(mCardsOut.at(0));
  if (mCardsOut.at(1)) mCards.insert(mCardsOut.at(1));
  mCardsOut.clear();
}

///////////////////////////////////////////////////////////////////////////////
// build array with cards offsets and indicies
// at least 28 ints (14 int pairs); return # of used ints; the last int is -1
// result: ofs, cardNo, ..., -1
int Player::buildHandXOfs (QList<CardPosInfo>& offsets, int startX, bool opened)
{
  CardList lst = mCards.sorted();

  int suitsCount = lst.countSuits();
  int cardsCount = lst.count();
  int suitOffset = opened ? ((mPlayerNo == 1) ? SUIT_OFFSET_P1 : SUIT_OFFSET) : CLOSED_CARD_OFFSET;
  int cardOffset = opened ? ((mPlayerNo == 1) ? CARD_OFFSET_P1 : CARD_OFFSET) : CLOSED_CARD_OFFSET;
  int cardsWidth = (cardsCount - suitsCount) * cardOffset + (suitsCount - 1) * suitOffset + CARD_WIDTH;

  if (mPlayerNo == 3)
    startX -= cardsWidth;
  else if (mPlayerNo == 1)
    startX -= cardsWidth / 2;

  Card* prev = NULL;
  for(int i = 0; i < lst.size(); i++)
  {
    Card* pCard = lst.at(i);
    if (!pCard)
        continue;
    if (prev)
    {
      bool suitChanged = prev->suit() != pCard->suit();
      int offset = suitChanged ? suitOffset : cardOffset;
      startX += offset;
    }
    offsets.push_back( CardPosInfo( mCards.indexOf(pCard), startX ) );
    prev = pCard;
  }

  return cardsCount;
}


int Player::cardAt(int lx, int ly, bool opened)
{
  int left, top;
  mDeskView->getLeftTop(mPlayerNo, left, top);
  QList<CardPosInfo> offsets;
  buildHandXOfs(offsets, left, opened);

  int res = -1;
  foreach( const CardPosInfo& pi, offsets )
  {
    int x1 = pi.offset, y1 = top;
    int x2 = x1 + CARD_WIDTH, y2 = y1 + CARD_HEIGHT;
    if (x1 < lx && lx < x2 && y1 < ly && ly < y2)
        res = pi.idx;
  }
  return res;
}


void Player::drawAt (int left, int top, int selNo)
{
  QList<CardPosInfo> offsets;
  int cnt = buildHandXOfs(offsets, left, !invisibleHand());
  if (!cnt)
    return;
  left = offsets.begin()->offset;
  foreach( const CardPosInfo& pi, offsets )
  {
    int x = pi.offset, y = top;
    Card *card = mCards.at(pi.idx);
    mDeskView->drawCard(card, x, y, !invisibleHand(), pi.idx == selNo);
  }

  mDeskView->update(left - 100, top - 10, offsets.last().offset - left + CARD_WIDTH + 100, CARD_HEIGHT + 10);
}


void Player::draw()
{
  int left, top;
  Q_ASSERT(mDeskView);
  mDeskView->getLeftTop(mPlayerNo, left, top);
  drawAt(left, top, mPrevHiCardIdx);
  mDeskView->drawPlayerMessage(mPlayerNo, mMessage, mPlayerNo!=m_model->mPlayerHi);
}


void Player::clearCardArea()
{
  if (!mDeskView)
      return;
  int left, top;
  mDeskView->getLeftTop(mPlayerNo, left, top);
  QList<CardPosInfo> offsets;
  int cnt = buildHandXOfs(offsets, left, !invisibleHand());
  if (!cnt)
      return;
  foreach( const CardPosInfo& pi, offsets )
  {
    int x = pi.offset, y = top;
    mDeskView->ClearBox(x, y, CARD_WIDTH, CARD_HEIGHT);
  }
  mDeskView->update(left - 100, top - 10, offsets.last().offset - left + CARD_WIDTH + 10, CARD_HEIGHT + 10);
}


void Player::highlightCard (int lx, int ly) {
  Q_UNUSED(lx)
  Q_UNUSED(ly)
  mPrevHiCardIdx = -1;
}

bool Player::isValidMove(const Card *move, const Card *lMove,
      const Card *rMove, const int trump)
{
  // Preferans-specific code
  if (!(lMove || rMove))
    return true; // first move is not restricted
  const Card *Validator = lMove ? lMove : rMove;
  if ((Validator->suit() == move->suit()) || (!mCards.minInSuit(Validator->suit())
    && (move->suit() == trump || ((!mCards.minInSuit(trump)))))) {
      return true;
  } else {
    mDeskView->model()->showMoveImpossible(true);
    return false;
  }
}
