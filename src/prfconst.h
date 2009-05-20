#ifndef PRFCONST_H
#define PRFCONST_H

#include <QString>


#define itoa(i,c,r) sprintf(c,"%i",i)
/*
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
*/

#define FONTSIZE 10

#define PEVENTMILLISECOND 50
#define CARDINCOLODA 32
#define MAXMASTLEN   8
#define CARDWIDTH    71
#define CARDHEIGHT   96
#define MAXGAMELEN   1024

#define SECSTDWAIT 1


enum TSide {
  left=0,
  right,
  noside
};

enum TMast {
  pica=1,
  trefa,
  bubna,
  cherva,
  withoutmast,
};

enum TGamesType {
  zerogame = 0,
  g86catch=56,
  raspass=57,
  vist=58,
  undefined=59,
  pass=60,
  g61=61,g62,g63,g64,g65,
  g71=71,g72,g73,g74,g75,
  g81=81,g82,g83,g84,g85,g86,
  g91=91,g92,g93,g94,g95,
  g101=101,g102,g103,g104,g105
};// g86 - miser

typedef struct {
  int   vzatok;
  int   perehvatov;
  int   len;
  int   sum;
}  TMastTable;
//-----------------------------------------------------------------------------
extern int g61stalingrad;
extern int g10vist;
extern int globvist;
extern int nBuletScore;
extern TGamesType CurrentGame;

//-----------------------------------------------------------------------------
int  NextGame(TGamesType);
const char *sGameName(TGamesType);
int nGetKoz(void);
int nGetGameCard(TGamesType gType);
int nGetVistCard(TGamesType gType);
int nGetMinCard4Vist(TGamesType gType);
int nGetGamePrice(TGamesType gType);
TGamesType GamesTypeByName(const QString &s);
int Card2Int(void *);

#endif
