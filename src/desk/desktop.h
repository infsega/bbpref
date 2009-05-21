#ifndef DESKTOP_H
#define DESKTOP_H

#include <QObject>

#include "card.h"
#include "ctlist.h"
#include "coloda.h"
#include "player.h"


class TDeskTop : public QObject {
  Q_OBJECT

public:
  TDeskTop ();
  TDeskTop (TDeskView *_DeskView);
  virtual ~TDeskTop ();

  Player *InsertGamer (Player *);
  void RunGame ();
  void Repaint ();
  int nPlayerTakeCards (TCard *p1, TCard *p2, TCard *p3, int koz);
  void ShowPaper ();
  int SaveGame (const QString &name);
  int LoadGame (const QString &name);
  void CloseBullet ();

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
  TColoda *Coloda;
  Tclist  *Gamers;
  Tncounter nCurrentStart,nCurrentMove;
  TCard *FirstCard,*SecondCard,*TherdCard;
  TCard *CardOnDesk[4];

  //int nBuletScore;
  int nflShowPaper;

private:
  void InternalConstruct ();
  //void insertscore (Tclist *, char *);
  //void replace (char *, char, char);
  Player *GetGamerByNum (int);
  int GetGamerWithMaxBullet (); // exept closed gamers
  TCard *ControlingMakemove (TCard *, TCard *);
  TCard *PipeMakemove (TCard *lMove, TCard *rMove);
  void GamerAssign (Player *, Player *);

  void drawInGameCard (int mPlayerNo, TCard *Card);

private:
  bool mPlayingRound;
  int mPlayerActive; // ��� ������ (���� �� ������� � mPlayingRound=true)
};


QString Tclist2pchar (Tclist *);


#endif
