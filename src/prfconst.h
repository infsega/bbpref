#ifndef PRFCONST_H
#define PRFCONST_H

#include <QString>
#include <QList>


typedef QList<int>  QIntList;
//typedef QList<int>  QIntList;


#define CARDWIDTH    71
#define CARDHEIGHT   96
/*
#define CARDWIDTH    56
#define CARDHEIGHT   67
*/
#define FONTSIZE 10

#define SUIT_OFFSET         22
#define NEW_SUIT_OFFSET     ((CARDWIDTH)+8)
#define CLOSED_CARD_OFFSET  ((CARDWIDTH)*2/3)

//#define PEVENTMILLISECOND 50
#define CARDINCOLODA 32
#define MAXMASTLEN   8

#define MAXGAMELEN   1024


enum eHand {
  LeftHand=0,
  RightHand,
  NoHand
};

enum eSuit {
  SuitSpades=1,
  SuitClubs,
  SuitDiamonds,
  SuitHearts,
  SuitNone,
};

enum eGameBid {
  zerogame=0,
  showpool=1,
  g86catch=56,
  raspass=57,
  whist=58,
  halfwhist=59,
  undefined=55,
  gtPass=60,
  g61=61,g62,g63,g64,g65,
  g71=71,g72,g73,g74,g75,
  g81=81,g82,g83,g84,g85,g86,
  g91=91,g92,g93,g94,g95,
  g101=101,g102,g103,g104,g105
};// g86 - miser


typedef struct {
  int tricks;
  int perehvatov;
  int len;
  int sum;
} tSuitProbs;


extern bool optStalingrad;
extern bool opt10Whist;
extern bool optWhistGreedy;
extern int optMaxPool;
extern QString optHumanName;
extern QString optPlayerName1;
extern bool optAlphaBeta1;
extern QString optPlayerName2;
extern bool optAlphaBeta2;
extern bool optDebugHands;
extern eGameBid gCurrentGame;
extern bool optAggPass;
extern int optPassCount;

extern bool optDealAnim;
extern bool optTakeAnim;
extern bool optPrefClub;

int succBid (eGameBid game);
const QString &sGameName (eGameBid game);
int trumpSuit ();
int gameTricks (eGameBid gType);
int gameWhists (eGameBid gType);
int gameWhistsMin (eGameBid gType);
int gamePoolPrice (eGameBid gType);
eGameBid gameName2Type (const QString &s);


#endif
