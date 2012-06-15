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
#define CARD_OFFSET_P1      (CARD_WIDTH - 10)
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

Player::~Player()
{
  clear();
}

inline void Player::internalInit()
{
  clear();
}

void Player::clear()
{
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

inline bool Player::invisibleHand () const
{
  /// @todo should be dispatched by model
  return mDeskView->optDebugHands ? false : mInvisibleHand;
}

inline void Player::setInvisibleHand (bool invis)
{
  mInvisibleHand = invis;
}

inline void Player::setCurrentStart (bool start)
{
  mIStart = start;
}

void Player::returnDrop()
{
  if (mCardsOut.at(0))
    mCards.insert(mCardsOut.at(0));
  if (mCardsOut.at(1))
    mCards.insert(mCardsOut.at(1));
  mCardsOut.clear();
}

///////////////////////////////////////////////////////////////////////////////
// build array with cards offsets and indicies
// at least 28 ints (14 int pairs); return # of used ints; the last int is -1
// result: ofs, cardNo, ..., -1
int Player::buildHandXOfs(QList<CardPosInfo>& offsets, int startX, int endX, bool opened)
{
  CardList lst = mCards.sorted();
  if (!opened)
  {
    foreach( Card* c, mDealedCards )
      lst.remove(c);
    lst.clearNulls();
    foreach( Card* c, mDealedCards )
      lst.insert(c);
  }

  int suitsCount = lst.countSuits();
  int cardsCount = lst.count();
  int suitOffset = opened ? ((mPlayerNo == 1) ? SUIT_OFFSET_P1 : SUIT_OFFSET) : CLOSED_CARD_OFFSET;
  int cardOffset = opened ? ((mPlayerNo == 1) ? CARD_OFFSET_P1 : CARD_OFFSET) : CLOSED_CARD_OFFSET;
  int cardOffsetCount = (cardsCount - suitsCount);
  int suitOffsetCount = (suitsCount - 1);
  int offsetsCount = cardsCount - 1;
  int cardsWidth = cardOffsetCount * cardOffset + suitOffsetCount * suitOffset + CARD_WIDTH;

  int roomWidth = endX - startX;
  if (roomWidth < cardsWidth)
  {
    int corr = (cardsWidth - roomWidth + offsetsCount - 1) / offsetsCount; // round up
    suitOffset -= corr;
    cardOffset -= corr;
    cardsWidth -= corr * offsetsCount;
  }

  int startFrom = startX;
  switch(mPlayerNo)
  {
  case 1:
    startFrom = (startX + endX - cardsWidth) / 2;
    break;
  case 2:
    startFrom = startX;
    break;
  case 3:
    startFrom = endX - cardsWidth;
    break;
  }

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
      startFrom += offset;
    }
    offsets.push_back( CardPosInfo( mCards.indexOf(pCard), startFrom ) );
    prev = pCard;
  }

  return cardsCount;
}

int Player::cardAt(int lx, int ly, bool opened)
{
  int left, right, top;
  mDeskView->getPlayerCardRoom(mPlayerNo, left, right, top);
  QList<CardPosInfo> offsets;
  buildHandXOfs(offsets, left, right, opened);

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

void Player::drawAt(int left, int right, int top, int selNo)
{
  QList<CardPosInfo> offsets;
  int cnt = buildHandXOfs(offsets, left, right, !invisibleHand());
  if (!cnt)
    return;
  left = offsets.begin()->offset;
  foreach( const CardPosInfo& pi, offsets )
  {
    int x = pi.offset, y = top;
    Card *card = mCards.at(pi.idx);
    if ( mDealedCards.contains(card) )
      continue;
    mDeskView->drawCard(card, x, y, !invisibleHand(), pi.idx == selNo);
  }

  mDeskView->update();
}

void Player::draw()
{
  int left, top, right;
  Q_ASSERT(mDeskView);
  mDeskView->getPlayerCardRoom(mPlayerNo, left, right, top);
  drawAt(left, right, top, mPrevHiCardIdx);
  mDeskView->drawPlayerMessage(mPlayerNo, mMessage, mPlayerNo!=m_model->mPlayerHi);
}

void Player::clearCardArea()
{
  if (!mDeskView)
      return;
  int left, right, top;
  mDeskView->getPlayerCardRoom(mPlayerNo, left, right, top);
  mDeskView->ClearBox(left, top, right - left, CARD_HEIGHT);
  mDeskView->update();
}

void Player::highlightCard (int lx, int ly)
{
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

void Player::getCardPos(const Card *card, int& x, int& y)
{
  if (!mDeskView)
    return;
  int left, right;
  mDeskView->getPlayerCardRoom(mPlayerNo, left, right, y);
  QList<CardPosInfo> offsets;
  buildHandXOfs(offsets, left, right, !invisibleHand());
  foreach( const CardPosInfo& pi, offsets )
  {
    if (card == mCards.at(pi.idx))
    {
      x = pi.offset;
      return;
    }
  }
}

void Player::setThinking()
{
  if (isHuman())
    setMessage( m_model->tr("your turn..."));
  else
    setMessage( m_model->tr("thinking..."));
}

void Player::clearMessage()
{
  setMessage("");
}
