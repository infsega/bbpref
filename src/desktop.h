#ifndef DESKTOP_H
#define DESKTOP_H

#include <QObject>

#include "card.h"
#include "ctlist.h"
#include "coloda.h"
#include "gamer.h"


class TDeskTop : public QObject {
  Q_OBJECT

public:
  TDeskTop ();
  TDeskTop (TDeskView *_DeskView);
  virtual ~TDeskTop ();

  TGamer *InsertGamer (TGamer *);
  void RunGame ();
  void Repaint (int left, int right, int top, int bottom);
  void Repaint ();
  void Repaint (int nGamer); // Call Repaint for i Gamers
  //void OnlyMessage (int nGamer);
  void ShowCard (int nGamer, TCard *Card);
  int nPlayerTakeCards (TCard *p1, TCard *p2, TCard *p3, int koz);
  void ShowPaper ();
  void RepaintCardOnDesk ();
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
  TGamer *GetGamerByNum (int);
  int GetGamerWithMaxBullet (); // exept closed gamers
  TCard *ControlingMakemove (TCard *, TCard *);
  TCard *PipeMakemove (TCard *lMove, TCard *rMove);
  void GamerAssign (TGamer *, TGamer *);
};


QString Tclist2pchar (Tclist *);


#endif
