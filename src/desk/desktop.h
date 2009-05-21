#ifndef DESKTOP_H
#define DESKTOP_H

#include <QObject>

#include "card.h"
#include "deck.h"
#include "player.h"


class PrefDesktop : public QObject {
  Q_OBJECT

public:
  PrefDesktop ();
  PrefDesktop (TDeskView *_DeskView);
  virtual ~PrefDesktop ();

  Player *InsertGamer (Player *);
  void RunGame ();
  void Repaint (bool emitSignal=true);
  int nPlayerTakeCards (Card *p1, Card *p2, Card *p3, int koz);
  void ShowPaper ();
  int SaveGame (const QString &name);
  int LoadGame (const QString &name);
  void CloseBullet ();

  inline Player *currentPlayer () const { return mPlayers[nCurrentMove.nValue]; }

  // Protocol
  time_t t;
  struct tm *tblock;
  int flProtocol;
  char ProtocolFileName[1024];
  char ProtocolBuff[1024];
  FILE *ProtocolFile;
  int OpenProtocol ();
  int CloseProtocol ();
  int WriteProtocol (const char *line);

  void emitRepaint ();

signals:
  void deskChanged ();

public:
  TDeskView *DeskView;
  Deck *deck;
  QList<Player *> mPlayers;
  Tncounter nCurrentStart,nCurrentMove;
  Card *FirstCard,*SecondCard,*TherdCard;
  Card *CardOnDesk[4];

  int nflShowPaper;

private:
  void InternalConstruct ();
  Player *player (int);
  Card *ControlingMakemove (Card *, Card *);
  Card *PipeMakemove (Card *lMove, Card *rMove);
  void GamerAssign (Player *, Player *);

  void drawInGameCard (int mPlayerNo, Card *card);

private:
  void drawBidWindows (const eGameBid *bids, int curPlr);
  void getPMsgXY (int plr, int *x, int *y);

  int playerWithMaxPool (); // except the players who closed the pool

private:
  bool mPlayingRound;
  int mPlayerActive; // кто играет (если не распасы и mPlayingRound=true)
  int iMoveX, iMoveY;
};


#endif
