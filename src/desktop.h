#ifndef DESKTOP_H
#define DESKTOP_H

#include <limits.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#include "card.h"
#include "ctlist.h"
#include "coloda.h"
#include "gamer.h"
//----------------------------------------------------
class TDeskTop {
  public:
    TDeskView *DeskView;
    TColoda *Coloda;
    Tclist  *Gamers;
    Tncounter nCurrentStart,nCurrentMove;
    TCard *FirstCard,*SecondCard,*TherdCard;
    TCard *CardOnDesk[4];
    TDeskTop(void);
    TDeskTop(TDeskView *_DeskView);
    ~TDeskTop();
    TGamer * InsertGamer(TGamer *);
    void RunGame(void);
    void Repaint(int ,int ,int,int);
    void Repaint (void);
    void Repaint (int ); // Call Repaint for i Gamers
    void OnlyMessage(int );
    void ShowCard(int nGamer,TCard *Card);
    int nPlayerTakeCards(TCard *p1,TCard *p2,TCard *p3,int koz );
    //     int nBuletScore;
    int nflShowPaper;
    void ShowPaper(void);
    void RepaintCardOnDesk(void);
    int SaveGame(const QString &name);
    int LoadGame(const QString &name);

    void CloseBullet(void);

    // Protocol
                time_t t;
                struct tm *tblock;
    int flProtocol;
    char ProtocolFileName[1024];
                char ProtocolBuff[1024];
    FILE *ProtocolFile;
                int OpenProtocol();
                int CloseProtocol();
                int WriteProtocol(char *);


  private:
    void InternalConstruct(void);
    void insertscore(Tclist *,char *);
    void   replace(char *,char , char);
    TGamer *GetGamerByNum(int );
    int GetGamerWithMaxBullet(void); // exept closed gamers
    TCard *ControlingMakemove(TCard *,TCard *);
    TCard *PipeMakemove(TCard *lMove,TCard *rMove);
    void GamerAssign(TGamer *,TGamer *);
};

//----------------------------------------------------
char *Tclist2pchar(Tclist *,char *);
//----------------------------------------------------

#endif
