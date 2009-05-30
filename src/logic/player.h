#ifndef PLAYER_H
#define PLAYER_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"


class Player {
public:
  Player (int aMyNumber, DeskView *aDeskView=0);
  Player (const Player &pl);
  virtual ~Player ();

  Player &operator = (const Player &pl);

  virtual void clear ();

  virtual bool invisibleHand () const;
  virtual void setInvisibleHand (bool invis);

  virtual void sortCards ();

  virtual void dealCard (Card *aCard); // �������� ������� �����

  virtual Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) = 0; //���
  virtual eGameBid moveBidding (eGameBid lMove, eGameBid rMove) = 0; //��� ��� ��������
  virtual eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist) = 0; // ����� ��������� ������� ������� -- ���� ��� ����
  virtual eGameBid dropForGame () = 0; // ����� ��� ����
  virtual eGameBid dropForMisere () = 0; // ����� ��� ������
  virtual void returnDrop (); // ������� �����

  virtual void getLeftTop (int *left, int *top);
  virtual void draw ();
  virtual void hilightCard (int lx, int ly); // ���������� ����� �� ������ ����������� (� ������������ ����, ���� ����)

  inline void setMessage (const QString &msg) { mMessage = msg; }
  inline const QString &message () const { return mMessage; }

  inline int number () const { return mPlayerNo; }

  inline eGameBid myGame () const { return mMyGame; }
  inline void setMyGame (eGameBid game) { mMyGame = game; }

  inline int tricksTaken () const { return mTricksTaken; }
  virtual void gotTrick ();
  virtual void gotPassPassTricks (int cnt);

protected:
  virtual void internalInit ();
  virtual void clone (const Player *pl);

  virtual void drawAt (DeskView *aDeskView, int left, int top, int selNo=-1);
  virtual void clearCardArea ();

  // at least 28 ints (14 int pairs); return # of used ints; the last int is -1
  // result: ofs, cardNo, ..., -1
  virtual int buildHandXOfs (int *dest, int startX, bool opened);
  virtual int cardAt (int lx, int ly, bool opened=true);

public:
  DeskView *mDeskView;

  ScoreBoard mScore;
  CardList mCards; // ���
  CardList mLeft;  // ���������� ����� (�������������� ��� ��������)
  CardList mRight; // ������ (�������������� ��� ��������)
  CardList mOut;   // ���� (��� ��� ��������������)
  CardList mCardsOut; // �� ������� ���
  // this part for miser catches
  Card *mCardCarryThru;

  int mClickX, mClickY;
  bool mWaitingForClick;

protected:
  QString mNick;
  QString mMessage;
  int mPlayerNo; // ��� �����
  bool mInvisibleHand;
  eGameBid mMyGame;
  int mTricksTaken;
  int mPrevHiCardIdx;
};


#endif
