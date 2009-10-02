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

#ifndef PLSCORE_H
#define PLSCORE_H

#include <QString>

#include "prfconst.h"


class ScoreBoard {
public:
  ScoreBoard ();
  ~ScoreBoard ();

  int recordScores (eGameBid aGamerType, eGameBid aMyType, int nGamerVz, int nMyVz, int nGamer, int myNumber, int nqVist);

  inline int pool () const { return mPool[mPool.size()-1]; }
  inline int mountain () const { return mMountain[mMountain.size()-1]; }
  inline int leftWhists () const { return mLeftWhists[mLeftWhists.size()-1]; }
  inline int rightWhists () const { return mRightWhists[mRightWhists.size()-1]; }

  int poolAdd (int delta);
  void whistsAdd (int index, int myNumber, int delta);
  void mountainDown (int delta);
  void mountainAmnesty (int delta);
  void mountainUp (int delta);

  QString poolStr (int maxItems=-1) const;
  QString mountainStr (int maxItems=-1) const;
  QString leftWhistsStr (int maxItems=-1) const;
  QString rightWhistsStr (int maxItems=-1) const;
  inline QString whistsStr () const { return QString::number(mWhists); }

  inline int whists () const { return mWhists; }
  inline void setWhists (int w) { mWhists = w; }

  void serialize (QByteArray &ba);
  bool unserialize (QByteArray &ba, int *pos);

private:
  QIntList mPool;
  QIntList mMountain;
  QIntList mLeftWhists;
  QIntList mRightWhists;
  int mWhists;
};


#endif
