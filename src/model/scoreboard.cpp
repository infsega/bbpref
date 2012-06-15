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

#include "scoreboard.h"

#include <QDebug>

#include "baser.h"
#include "prfconst.h"
#include "desktop.h"
#include "player.h"

static QString intList2Str (const QIntList &list, int maxItems)
{
  QString res;
  int st = 1, len = list.size();
  if (maxItems == 0)
    return QString();
  if (maxItems > 0 && len > maxItems)
    st += (maxItems - len);
  for (int f = st; f < list.size(); f++)
  {
    res += QString::number(list[f]);
    res += '.';
  }
  return res;
}

///////////////////////////////////////////////////////////////////////////////
ScoreBoard::ScoreBoard (PrefModel *model) : m_model(model)
{
  mPool << 0;
  mMountain << 0;
  mLeftWhists << 0;
  mRightWhists << 0;
  mScoreInWhists = 0;
}

ScoreBoard::~ScoreBoard ()
{
}

int ScoreBoard::poolAdd(int delta)
{
  int score = pool();
  int ns = score+delta;
  if (ns <= m_model->optMaxPool)
  {
    mPool << ns;
    return 0;
  }
  else
  {
    if (m_model->optMaxPool != score)
      mPool << m_model->optMaxPool;
    return delta-(m_model->optMaxPool-score);
  }
}


void ScoreBoard::mountainDown (int delta)
{
  if (delta == 0)
  	return;
  int score = mountain()-delta;
  if (score >= 0)
  {
    mMountain << score;
  }
  else
  {
    score = (delta-mountain())*10/2;
    if (mountain()) mMountain << 0;
    mLeftWhists << leftWhists()+score;
    mRightWhists << rightWhists()+score;
  }
}

void ScoreBoard::mountainAmnesty (int delta)
{
  if (delta == 0)
    return;
  mMountain.last() -= delta;
  if ((mMountain.last() == 0) || (mMountain.at(mMountain.size()-2) == mMountain.last()))
    mMountain.removeLast();
}

void ScoreBoard::mountainUp (int delta)
{
  if (delta == 0)
  	return;
  int score = mountain();
  mMountain << score+delta;
}

void ScoreBoard::whistsAdd (int index, int myNumber, int delta)
{
  QIntList *dList;
  int score;
  dList = ((myNumber%3)+1 == index) ? &mLeftWhists : &mRightWhists;
  int pnCurrent = dList->at(dList->size()-1);
  score = pnCurrent+delta*10;
  dList->append(score);
}

int ScoreBoard::recordScores
(
  eGameBid aGamerType,
  eGameBid aMyType,
  int nGamerVz,
  int nMyVz,
  int nGamer,
  int myNumber,
  int nqVist
  )
{
  int nGamePrice = gamePoolPrice(aGamerType);
  int nGameCard = gameTricks(aGamerType);
  int nVistCard = m_model->gameWhists(aGamerType);
  int score = 0, pnCurrent = 0;
  QIntList *dList;

  if (aMyType >= g61 && aMyType != g86)
  {
    if (nGamerVz >= nGameCard)
      return poolAdd(nGamePrice);
    else
    {
      pnCurrent = mMountain[mMountain.size()-1];
      score = nGamePrice*(nGameCard-nMyVz)+pnCurrent;
      if (score)
        mMountain << score;
    }
    return 0;
  }

  //-------------------------------------------------------------------------
  if (aMyType == g86)
  {
    if (nMyVz)
    {
      mountainUp(nGamePrice*nMyVz);
      return 0;
    }
    return poolAdd(10);
  }

  //-------------------------------------------------------------------------
  if (aMyType == raspass)
  {
    pnCurrent = mMountain[mMountain.size()-1];
    score = nGamePrice*(nMyVz)+pnCurrent;
    if (nMyVz && score)
    {
      mMountain << score;
      return 0;
    }
    if (!nMyVz)
      return poolAdd(1);
  }

  dList = ((myNumber%3)+1 == nGamer) ? &mLeftWhists : &mRightWhists;
  pnCurrent = dList->at(dList->size()-1);

  //-------------------------------------------------------------------------
  if (aMyType == gtPass && aGamerType != g86)
  {
    if (!m_model->optWhistGreedy && nGameCard-nGamerVz > 0)
    {
      score = nGamePrice*((nGameCard-nGamerVz)+nMyVz)+pnCurrent;
    }
    if (score)
      dList->append(score);
  }

  if (aMyType == halfwhist && aGamerType != g86)
  {
    score = nGamePrice*nMyVz+pnCurrent;
    if (score)
      dList->append(score);
  }

  //-------------------------------------------------------------------------
  if (aMyType == whist && aGamerType != g86)
  {
    if (nGameCard-nGamerVz > 0)
    {
      if (nqVist == 2)
        score = nGamePrice*((nGameCard-nGamerVz)+nMyVz)+pnCurrent;
      else
        score = nGamePrice*((nGameCard-nGamerVz)+nMyVz)+pnCurrent;
    }
    else
    {
      if (nqVist == 2)
        score = nGamePrice*(nMyVz)+pnCurrent;
      else
        score = nGamePrice*(10-nGamerVz)+pnCurrent;

      if (nVistCard > 10-nGamerVz)
      {
        if (nqVist == 2)
        {
          double d = nVistCard;
          d = (d/2-nMyVz)*nGamePrice;
          mountainUp((int)d);
        }
        else
        {
          if (nVistCard-nMyVz > 0)
            mountainUp(nGamePrice*(nVistCard-(10-nGamerVz)));
        }
      }
    }
    if (score)
      dList->append(score);
  }
  return 0;
}


QString ScoreBoard::poolStr (int maxItems) const
{
  QString str = intList2Str(mPool, maxItems);
  if (mPool.last() == m_model->optMaxPool)
    str += "<<";
  return str;
}

QString ScoreBoard::mountainStr (int maxItems) const { return intList2Str(mMountain, maxItems); }
QString ScoreBoard::leftWhistsStr (int maxItems) const { return intList2Str(mLeftWhists, maxItems); }
QString ScoreBoard::rightWhistsStr (int maxItems) const { return intList2Str(mRightWhists, maxItems); }


static void serializeIntList (QByteArray &ba, const QIntList &lst)
{
  serializeInt(ba, lst.size());
  for (int f = 0; f < lst.size(); f++)
    serializeInt(ba, lst[f]);
}

static bool unserializeIntList (QByteArray &ba, int *pos, QIntList &lst)
{
  int t;
  lst.clear();
  if (!unserializeInt(ba, pos, &t))
    return false;
  while (t-- > 0)
  {
    int c;
    if (!unserializeInt(ba, pos, &c))
      return false;
    lst << c;
  }
  return true;
}

void ScoreBoard::serialize (QByteArray &ba)
{
  qDebug() << "Pool";
  serializeIntList(ba, mPool);
  qDebug() << "Mountain";
  serializeIntList(ba, mMountain);
  qDebug() << "Left Whists";
  serializeIntList(ba, mLeftWhists);
  qDebug() << "Right Whists";
  serializeIntList(ba, mRightWhists);
  qDebug() << "Score in Whists";
  serializeInt(ba, mScoreInWhists);
}

bool ScoreBoard::unserialize (QByteArray &ba, int *pos)
{
  qDebug() << "Pool";
  if (!unserializeIntList(ba, pos, mPool)) return false;
  qDebug() << "Mountain";
  if (!unserializeIntList(ba, pos, mMountain)) return false;
  qDebug() << "LeftWhists";
  if (!unserializeIntList(ba, pos, mLeftWhists)) return false;
  qDebug() << "RightWists";
  if (!unserializeIntList(ba, pos, mRightWhists)) return false;
  qDebug() << "ScoreInWhists";
  if (!unserializeInt(ba, pos, &mScoreInWhists)) return false;
  return true;
}


void ScoreBoard::calculateScore(PrefModel *model, int nPassCounter)
{
    const int mPlayerActive = model->activePlayerNumber();
    Player *plr;
    if (mPlayerActive)
        plr = model->player(mPlayerActive);
    else
        plr = 0;

    for (int i = 1; i <= 3;i++ ) {
      Player *player_i = model->player(i);
      int RetValAddRec = player_i->mScore.recordScores(model->currentGame(), player_i->game(),
        plr ? plr->tricksTaken() : 0, player_i->tricksTaken(), plr ? mPlayerActive : 0,
        i, 2-nPassCounter);
      if (RetValAddRec) {
        int index = model->playerWithMaxPool();
        if (index) {
          player_i->mScore.whistsAdd(index, i, RetValAddRec); //
          RetValAddRec = model->player(index)->mScore.poolAdd(RetValAddRec); //
          if (RetValAddRec) {
            index = model->playerWithMaxPool();
            if (index) {
              player_i->mScore.whistsAdd(index, i, RetValAddRec); //
              RetValAddRec = model->player(index)->mScore.poolAdd(RetValAddRec);
              if (RetValAddRec) player_i->mScore.mountainDown(RetValAddRec);
            } else {
              player_i->mScore.mountainDown(RetValAddRec);
            }
          }
        } else {
          player_i->mScore.mountainDown(RetValAddRec);
        }
      }
    }

	//amnesty for pass out (doesn't matter for score)
    if (model->currentGame() == raspass)
    {
		int mm=10;
		int m=0;
        for (int i=1; i<=3; i++)
        {
			m = model->player(i)->tricksTaken();
			if (m<mm) mm = m;
		}
		if (mm !=0)
			for (int i=1; i<=3; i++)
				model->player(i)->mScore.mountainAmnesty(mm);
	}
}
