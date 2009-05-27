#ifndef DESKTOP_H
#define DESKTOP_H

#include <QByteArray>
#include <QObject>

#include "card.h"
#include "ncounter.h"
#include "player.h"


class PrefDesktop : public QObject {
  Q_OBJECT

public:
  PrefDesktop (DeskView *aDeskView=0);
  virtual ~PrefDesktop ();

  Player *addPlayer (Player *);

  void runGame ();

  void draw (bool emitSignal=true);
  void drawPool ();

  bool saveGame (const QString &name); // don't working!
  bool loadGame (const QString &name); // don't working!

  void closePool ();

  inline Player *currentPlayer () const { return mPlayers[nCurrentMove.nValue]; }

  void serialize (QByteArray &ba);
  bool unserialize (QByteArray &ba, int *pos);

  void emitRepaint ();

signals:
  void deskChanged ();

public:
  DeskView *mDeskView;
  CardList mDeck;
  QList<Player *> mPlayers;
  WrapCounter nCurrentStart, nCurrentMove;
  Card *mFirstCard, *mSecondCard, *mThirdCard;
  Card *mCardsOnDesk[4];

  bool mShowPool;
  bool mOnDeskClosed;

private:
  void internalInit ();

  Player *player (int num);
  Player *player (const WrapCounter &cnt);
  Card *makeGameMove (Card *lMove, Card *rMove);

  void drawInGameCard (int mCardNo, Card *card);
  void inGameCardLeftTop (int mCardNo, int *left, int *top);
  void animateDeskToPlayer (int plrNo);

private:
  //void drawBidWindows (const eGameBid *bids, int curPlr);
  void getPMsgXY (int plr, int *x, int *y);

  int playerWithMaxPool (); // except the players who closed the pool

  int whoseTrick (Card *p1, Card *p2, Card *p3, int koz);

private:
  bool mPlayingRound;
  int mPlayerActive; // кто играет (если не распасы и mPlayingRound=true)
  int iMoveX, iMoveY;
  int mPlayerHi; // подсвеченая мессага
};


#endif
