#include <QDebug>

#include <QTime>

#include <stdlib.h>
#include <stdio.h>

#include "prfconst.h"

#include "formbid.h"
#include "kpref.h"


#include "debug.h"
#include "deskview.h"
#include "player.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "aialphabeta.h"


// 1-33; 0: unused
/*
#define SUIT(c)  (((c)-1)%4)
#define FACE(c)  ((((c)-1)/4)+7)
#define CARD(face,suit)  (((face-7)*4+(suit))+1)
*/

typedef unsigned char  card_t;


static inline card_t CARD (int face, int suit) {
  if (face < 7 || face > 14 || suit < 0 || suit > 3) abort();
  return ((face-7)*4+suit)+1;
}


static inline int SUIT (int c) {
  return (c-1)%4;
}


static inline int FACE (int c) {
  return ((c-1)/4)+7;
}


static inline int compareCards (int c0, int c1) {
  int t0 = SUIT(c0), t1 = SUIT(c1);
  int r = t1-t0;
  if (!r) {
    t0 = FACE(c0), t1 = FACE(c1);
    r = t0-t1;
  }
  return r;
}


void xsortCards (card_t *arr, int len) {
  int f, c;
  for (f = 0; f < len; f++) {
    for (c = len-1; c > f; c--) {
      int cc = compareCards(arr[c], arr[f]);
      if (cc > 0) {
        int t = arr[c];
        arr[c] = arr[f];
        arr[f] = t;
      }
    }
  }
}


static int gTrumpSuit; // trump; global var
static int gPassOutSuit; // ������ ����� ��� ������� ��� ������� ����� ��������
//static int gGameBid; // >=6
//static int gMoves[4][10];
//static int gTricks[3];
//static int gE0, gE1, gMe;
//static int gWhoPlays;
static int gIterations;

//static int gTrk = 0;
static bool gPassOut;


typedef struct {
  int faces[10];
  int suits[10];
  int suitCount[4]; // # of cards in each suit
  int suitStart[4]; // 1st card of each suit
  int tricks;
} tHand;


//static long long xStTime, cTime;
static tHand xHands[3];
static int xCardsLeft;
static int xDeskFaces[3], xDeskSuits[3];
//static int lastMoveF, lastMoveS;
static QTime stTime;


//#define ABDEBUG
//#define ADVANCED_RES
/*
 * ����� ������ ���� ������������� �� ������ � ������� �������� "�����"
 * a: ����� player ������ �������� ��� �������
 * b: ����� player+1 ������ �������� ��� �������
 * c: ����� player+2 ������ �������� ��� �������
 * �������: �� �� �����
 *
 * ���� � ��������� ����� ������: http://clauchau.free.fr/gamma.html
 * idea and pseudocode was taken from here: http://clauchau.free.fr/gamma.html
 */
static void abcPrune (
  int turn, int player,
  int a, int b, int c,
  int *ra, int *rb, int *rc, int *rm
) {
    /*
     * (x, y, z) := some static additive evaluation of Position,
     * x measuring how good is Position for the Player to move,
     * y measuring how good is Position for the next Player,
     * z measuring how good is Position for the further next Player,
     * the higher the better, -infinite for a defeat, +infinite for a win;
     *
     * if the game is over then return (x, y, z, "game over");
     * else return ( min(x-y, x-z), min(y-x, y-z), min(z-x, z-y), "static" );
     */
/*
  if (!xCardsLeft) {
    gIterations++;
    if (gIterations%8000000 == 0) {
      cTime = getTimeMs();
      fprintf(stderr, "\r%i (%i seconds)\x1b[K", gIterations, (int)((cTime-xStTime)/1000));
    }
    *ra = xHands[player].tricks;
    *rb = xHands[(player+1)%3].tricks;
    *rc = xHands[(player+2)%3].tricks;
    return;
  }
*/

  //int lmF = lastMoveF, lmS = lastMoveS;
#ifdef ABDEBUG
  printf("cards left: %i; turn: %i\n", xCardsLeft, turn);
  printStatus(turn, player, 0);
#endif

  tHand *hand = &(xHands[player]);
  //int bestx = a, worsty = b, worstz = c;
  int bestx = -666, worsty = 666, worstz = 666;
  int bestm = -1;
  int n = 0; // will count equivalent moves
  int crdFace, crdSuit, tmp, who = -1;
  int newTurn = turn+1, newPlayer = (player+1)%3;
  int sDeskFaces[3], sDeskSuits[3];
  sDeskFaces[0] = xDeskFaces[0]; sDeskFaces[1] = xDeskFaces[1]; sDeskFaces[2] = xDeskFaces[2];
  sDeskSuits[0] = xDeskSuits[0]; sDeskSuits[1] = xDeskSuits[1]; sDeskSuits[2] = xDeskSuits[2];
  if (turn == 2) {
    newTurn = 0;
    --xCardsLeft;
  }
  int crdNo = 0, crdNext, ccc = 0;
  for (int f = 0; f < 10; f++) if (hand->faces[f]) ccc++;
  if (!ccc) {
    abort();
  }
  //int movesChecked = 0, firstm = -1;
  while (crdNo < 10) {
    crdFace = hand->faces[crdNo];
    if (!hand->faces[crdNo]) {
      crdNo++;
      continue;
    }
    crdNext = crdNo+1;
    crdSuit = hand->suits[crdNo];
    if (!turn) {
      // ������ ��� ����� ���� ����� ����, ���� ��� �� ������ � �� ������ ���� ��������
      if (gPassOutSuit >= 0 && crdSuit != gPassOutSuit && hand->suitCount[gPassOutSuit]) {
        // ��, ��� ����� ������� �����, ���Σ� � ������ �����
        tmp = hand->suitStart[gPassOutSuit];
        if (tmp == crdNo) abort(); // � ������ �� ������
        if (tmp < crdNo) break; // �� ��� � ��� �����, � ��� �ӣ, ��� ����, ���������
        // ������� � ��������� ������
        crdNo = tmp;
        continue;
      }
      goto doMove;
    }
    // check for valid move
    // ����� � ���������� �����?
    if (crdSuit == xDeskSuits[0]) goto doMove;
    // ��, �� �� �����; ��-��, �� � ��� �� ����� ����?
    // ������ ����� � ��� ����?
    if (hand->suitCount[xDeskSuits[0]]) {
      // ���� ����, ������ ��� ����� �գ��� �������; ����� ����� � ������ �����
      tmp = hand->suitStart[xDeskSuits[0]];
      if (tmp < crdNo) break; // �ӣ, ������ ������ ������
      if (tmp > crdNo) {
        // �������
        crdNo = tmp;
        continue;
      }
      // ��� ���� � �����
      goto doMove;
    }
    // ��, ������ ����� ���
    // � ������ ����?
    if (gTrumpSuit <= 3) {
      // ���� ��������, ���� ������?
      if (hand->suitCount[gTrumpSuit]) {
        // ���� ����
        tmp = hand->suitStart[gTrumpSuit];
        if (tmp < crdNo) break; // �ӣ, ������ ������ ������
        if (tmp > crdNo) {
          // �������
          crdNo = tmp;
          continue;
        }
        // ��� ���� � �����
        goto doMove;
      } else {
        // ��, � ������� ���, ����� ������ �� ������
        goto doMove;
      }
    } else {
      // ���� �����������, ��� ����� ����� ���ģ�, ����
      goto doMove;
    }
doMove:
/*
    movesChecked++;
    if (firstm < 0) firstm = crdNo;
*/
    // ���������� ������������������ �� ������ ��������� ���� ����� �����
    // ��������, ��� ��� ����� �������� �����, ����� �� ��� ������
    int scnt = hand->suitCount[crdSuit];
    if (scnt > 1) {
      // � ���� ����� ���� �ݣ �����
      // ��������, ���� �� � ���� �ݣ ��� �����
      if (xHands[0].suitCount[crdSuit]+xHands[1].suitCount[crdSuit]+xHands[2].suitCount[crdSuit] <= scnt) {
        // ������������ ������ �������� ���� �����; ������� ������ ���� ţ �����
        int tsuit = crdSuit+1;
        while (tsuit <= 3 && hand->suitCount[tsuit] == 0) tsuit++;
        crdNext = tsuit>3 ? 11 : hand->suitStart[tsuit];
      } else {
        // ����� ����� ���� �ݣ � ����-��
        int tface = crdFace+1;
        while (crdNext <= 10 && hand->suits[crdNext] == crdSuit && hand->faces[crdNext] == tface) {
          crdNext++;
          tface++;
        }
      }
    }
    // ������ ����� �� ����
    xDeskSuits[turn] = crdSuit;
    xDeskFaces[turn] = crdFace;
    // ������� ����� �� ����
    hand->suitCount[crdSuit]--;
    if (crdNo == hand->suitStart[crdSuit]) hand->suitStart[crdSuit]++;
    hand->faces[crdNo] = 0;

    //lastMoveF = crdFace; lastMoveS = crdSuit;
    int y, z, x;
    if (turn == 2) {
      // the turn is done, count tricks
      //who = whoTakes(pdesk, gTrumpSuit);
      // � ���, ����������, ������?
      if (gTrumpSuit <= 3) {
        // trump game
        if (xDeskSuits[0] == gTrumpSuit) {
          // ������� ��������
          who = 0; tmp = xDeskFaces[0];
          if (xDeskSuits[1] == xDeskSuits[0] && xDeskFaces[1] > tmp) { tmp = xDeskFaces[1]; who = 1; }
          if (xDeskSuits[2] == xDeskSuits[0] && xDeskFaces[2] > tmp) who = 2;
        } else if (xDeskSuits[1] == gTrumpSuit) {
          // ������ ��������
          who = 1; tmp = xDeskFaces[0];
          if (xDeskSuits[2] == xDeskSuits[1] && xDeskFaces[2] > tmp) who = 2;
        } else if (xDeskSuits[2] == gTrumpSuit) {
          // ������ ��������
          who = 2;
        } else {
          // ����� �� �������
          who = 0; tmp = xDeskFaces[0];
          if (xDeskSuits[1] == xDeskSuits[0] && xDeskFaces[1] > tmp) { tmp = xDeskFaces[1]; who = 1; }
          if (xDeskSuits[2] == xDeskSuits[0] && xDeskFaces[2] > tmp) who = 2;
        }
      } else {
        // notrump game
        who = 0; tmp = xDeskFaces[0];
        if (xDeskSuits[1] == xDeskSuits[0] && xDeskFaces[1] > tmp) { tmp = xDeskFaces[1]; who = 1; }
        if (xDeskSuits[2] == xDeskSuits[0] && xDeskFaces[2] > tmp) who = 2;
      }
      who = (who+player+1)%3;
      xHands[who].tricks++; // ��������� ������
#ifdef ABDEBUG
      printf("==%i takes; cards left: %i; turn: %i\n", who, xCardsLeft, turn);
      printStatus(turn, player, 1);
#endif
      if (xCardsLeft < 0) abort();
      if (!xCardsLeft) {
        // �ӣ, �����������, ����� ������������
        gIterations++;
        if (gIterations%1000000 == 0) {
          if (stTime.elapsed() >= 5000) {
            stTime.start();
            //cTime = getTimeMs();
            //fprintf(stderr, "\r%i (%i seconds)\x1b[K", gIterations, (int)((cTime-xStTime)/1000));
            fprintf(stderr, "\r%i\x1b[K", gIterations);
          }
        }
/*
        y = xHands[newPlayer].tricks;
        z = xHands[(newPlayer+1)%3].tricks;
        x = xHands[(newPlayer+2)%3].tricks;
*/
        x = xHands[player].tricks;
        y = xHands[newPlayer].tricks;
        z = xHands[(player+2)%3].tricks;
        if (gPassOut) {
          x = 10-x;
          y = 10-y;
          z = 10-z;
        }
#ifdef ADVANCED_RES
        if (player == gWhoPlays) {
          // � �����; ������� ��?
          if (x < gGameBid) {
            // ���, ����������
            x = -gGameBid-1; /*y = z = 666;*/
          } else {
            // ��, ���� ��ϣ
            x = 20+gGameBid;
/*
            switch (gGameBid) {
              case 6:
                if (y < 2) y = -666+y; // bad
                if (z < 2) z = -666+z; // bad
                break;
              case 7: case 8: case 9:
                if (y < 1) y = -666+y; // bad
                if (z < 1) z = -666+z; // bad
                break;
            }
*/
          }
        } else {
          // � ������; ���������� ��?
          if (xHands[gWhoPlays].tricks < gGameBid) {
            // �� ������ �������� �������
            //x = 666-(gGameBid-xHands[gWhoPlays].tricks); // �� �������
            // ����� � ����
            if (gWhoPlays == newPlayer) {
              //y = -xHands[gWhoPlays].tricks-1;
              //z = 666;
            } else {
              //z = -xHands[gWhoPlays].tricks-1;
              //y = 666;
            }
          } else {
            // ���, �����, ���, ���� ��ϣ; � �?
/*
            switch (gGameBid) {
              case 6:
                if (x < 2) x = -15-x; else x = 15+x;
                break;
              case 7: case 8: case 9:
                if (x < 1) x = -15-x; else x = 15+x;
                break;
            }
            if (gWhoPlays == newPlayer) {
              y = 666;
              switch (gGameBid) {
                case 6:
                  if (z < 2) z = -15-z; else z = 15+z;
                  break;
                case 7: case 8: case 9:
                  if (z < 1) z = -15-z; else z = 15+z;
                  break;
              }
            } else {
              z = 666;
              switch (gGameBid) {
                case 6:
                  if (y < 2) y = -15-y; else y = 15+y;
                  break;
                case 7: case 8: case 9:
                  if (y < 1) y = -15-y; else y = 15+y;
                  break;
              }
            }
*/
          }
        }
#endif
        //printStatus(2, player, 0);
      } else {
        // ���������� ��������� ������
        //abcPrune(newTurn, newPlayer, -c, -a, b, &y, &z, &x, NULL);
        if (who == player) {
          // � �� � ������, ����� �������
          abcPrune(0, player, a, b, c, &x, &y, &z, NULL);
        } else if (who == newPlayer) {
          // ��������� ������; ������� � ����������
          abcPrune(0, newPlayer, -c, -a, b, &y, &z, &x, NULL);
        } else {
          // ���������� ������; ��� ����� ��� ��������� �����; ��ԣ� ���������, ��
          abcPrune(0, who, -b, c, -a, &z, &x, &y, NULL);
        }
      }
      // ����� ������? ����������� ������ ���
      xHands[who].tricks--;
    } else {
      // ���������� ��������� ������
      abcPrune(newTurn, newPlayer, -c, -a, b, &y, &z, &x, NULL);
    }
    // ����������� ����
    xDeskFaces[0] = sDeskFaces[0]; xDeskFaces[1] = sDeskFaces[1]; xDeskFaces[2] = sDeskFaces[2];
    xDeskSuits[0] = sDeskSuits[0]; xDeskSuits[1] = sDeskSuits[1]; xDeskSuits[2] = sDeskSuits[2];
    // ���Σ� � ���� �����
    hand->suitCount[crdSuit]++;
    hand->faces[crdNo] = crdFace;
    if (crdNo+1 == hand->suitStart[crdSuit]) hand->suitStart[crdSuit]--;
    // ��������, �� �����
    if (bestm >= 0 && x == bestx) {
      // we've found an equivalent move
      //if (bestm < 0) abort();
      n++;
      if (y < worsty) worsty = y;
      if (z < worstz) worstz = z;
      //if (myrand()%n == n-1) bestm = crdNo;
      // hands are sorted, so take the smallest possible card
      if (bestm < 0 || crdFace < hand->faces[bestm]) bestm = crdNo;
    } else if (x > bestx) {
      // we've found a better move
      n = 1;
      bestm = crdNo;
      bestx = x; worsty = y; worstz = z;
      if (x > b || x > c) break; // �ӣ, ������ ������ �� ����, �ӣ ����� �� ������; goto done;
      if (x > a) a = x;
    }
    // ��ң� ��������� �����
    crdNo = crdNext;
  }
  xDeskFaces[0] = sDeskFaces[0]; xDeskFaces[1] = sDeskFaces[1]; xDeskFaces[2] = sDeskFaces[2];
  xDeskSuits[0] = sDeskSuits[0]; xDeskSuits[1] = sDeskSuits[1]; xDeskSuits[2] = sDeskSuits[2];
  if (turn == 2) {
    xCardsLeft++;
  }
  *ra = bestx; *rb = worsty; *rc = worstz;
  if (rm) *rm = bestm;
/*
  if (rm) *rm = bestm>=0?bestm:firstm;
  if (bestm < 0) {
    fprintf(stderr, "first: %i (%i)\n", firstm, movesChecked);
  }
*/
  //lastMoveF = lmF; lastMoveS = lmS;
}


//static card_t hands[3][10];



static const char *cFaceS[8] = {" 7"," 8"," 9","10"," J"," Q"," K"," A"};
static const char *cSuitS[4] = {"s","c","d","h"};

static void printHand (tHand *hand) {
  int z;
  for (z = 0; z < 10; z++) {
    if (hand->faces[z]) {
      printf(" %s%s(%2i)", cFaceS[hand->faces[z]-7], cSuitS[hand->suits[z]], CARD(hand->faces[z], hand->suits[z]));
    } else {
      printf(" ...");
    }
  }
  printf("  0:(%i,%i); 1:(%i,%i); 2:(%i,%i); 3:(%i,%i)",
    hand->suitCount[0], hand->suitStart[0],
    hand->suitCount[1], hand->suitStart[1],
    hand->suitCount[2], hand->suitStart[2],
    hand->suitCount[3], hand->suitStart[3]);
  printf("\n");
}


static void printDesk (int cnt) {
  printf("desk:");
  for (int z = 0; z < cnt; z++) {
    printf(" %s%s(%2i)", cFaceS[xDeskFaces[z]-7], cSuitS[xDeskSuits[z]], CARD(xDeskFaces[z], xDeskSuits[z]));
  }
  printf("\n");
}


/*
 * aLeftPlayer: next in turn
 * aRightPlayer: prev in turn
 * 0, 1th
 * 1th, 2nd
 */
Card *CheatPlayer::moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut) {
  card_t hands[3][10];
  card_t desk[3];
  int crdLeft = 0;
  int trumpSuit = 0;
  Player *movePlrObj = 0, *plst[3];

//again:
  plst[0] = plst[1] = plst[2] = 0;
  plst[mPlayerNo-1] = this;
  plst[aLeftPlayer->number()-1] = aLeftPlayer;
  plst[aRightPlayer->number()-1] = aRightPlayer;

  // build hands
  for (int c = 0; c < 3; c++) {
    if (!plst[c]) abort();
    CardList *clst = &(plst[c]->mCards);
    //for (int f = 0; f < 10; f++) hds[c][f] = hands[c][f] = 0;
    int pos = 0;
    for (int f = 0; f < clst->size(); f++) {
      Card *ct = clst->at(f);
      if (!ct) continue;
      hands[c][pos++] = CARD(ct->face(), ct->suit()-1);
      if (pos > crdLeft) crdLeft = pos;
    }
    for (int f = pos; f < 10; f++) hands[c][f] = 0;
    xsortCards(hands[c], pos);
  }

/*
  if (!lMove && !rMove && crdLeft == 10) {
    return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
  }
*/

  // find game
  eGameBid bid;
  if (mMyGame == gtPass || mMyGame == whist) {
    bid = aLeftPlayer->myGame();
    if (bid == gtPass || bid == whist) {
      bid = aRightPlayer->myGame();
      movePlrObj = aRightPlayer;
    } else movePlrObj = aLeftPlayer;
  } else {
    bid = mMyGame;
    movePlrObj = this;
  }

  gPassOut = (bid == g86 || bid == g86catch || bid == raspass);
  trumpSuit = bid%10-1;//(bid-(bid/10)*10)-1;
/*
  if (bid == g86catch || bid == g86 || bid == raspass) {
    return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
  }
*/
  if (bid == g86catch || bid == g86 || bid == raspass) {
    trumpSuit = 4;
  }
  if (trumpSuit < 0) trumpSuit = 4;

  fprintf(stderr, "po:%s; lm:%s, rm:%s\n", isPassOut?"y":"n", lMove?"y":"n", rMove?"y":"n");
  if (isPassOut && rMove && !lMove) {
    // ��� �������, ������ ��� ������ ����, ������ ���
    gPassOutSuit = rMove->suit()-1;
    fprintf(stderr, "pass-out: %i\n", gPassOutSuit);
    rMove = 0;
  } else gPassOutSuit = -1;

  // build desk
  int turn = 0;
  if (lMove) {
    desk[turn++] = CARD(lMove->face(), lMove->suit()-1);
    if (rMove) desk[turn++] = CARD(rMove->face(), rMove->suit()-1);
  } else if (rMove) {
    desk[turn++] = CARD(rMove->face(), rMove->suit()-1);
  }

  // build hands
  for (int f = 0; f < 3; f++) {
    xHands[f].suitCount[0] = xHands[f].suitCount[1] = xHands[f].suitCount[2] = xHands[f].suitCount[3] = 0;
    xHands[f].suitStart[0] = xHands[f].suitStart[1] = xHands[f].suitStart[2] = xHands[f].suitStart[3] = 11;
    xHands[f].tricks = plst[f]->tricksTaken();
    int st;
    for (int z = 0; z < 10; z++) {
      if (hands[f][z]) {
        xHands[f].faces[z] = FACE(hands[f][z]);
        st = xHands[f].suits[z] = SUIT(hands[f][z]);
        if (xHands[f].suitCount[st]++ == 0) xHands[f].suitStart[st] = z;
      } else xHands[f].faces[z] = 0;
    }
  }

  // build desk
  for (int f = 0; f < turn; f++) {
    xDeskFaces[f] = FACE(desk[f]);
    xDeskSuits[f] = SUIT(desk[f]);
  }

  int a, b, c, move;
  int me = this->number()-1;
  xCardsLeft = crdLeft;
  gTrumpSuit = trumpSuit;
  gIterations = 0;

  printf("%shand 0:", this->number()==0?"*":" ");
  printHand(&(xHands[0]));
  printf("%shand 1:", this->number()==1?"*":" ");
  printHand(&(xHands[1]));
  printf("%shand 2:", this->number()==2?"*":" ");
  printHand(&(xHands[2]));
  printDesk(turn);

  // �����������
/*
  if (turn > 0) {
    // ����� ������ �����?
    if (hands[me].suitCount(
  }
*/

  stTime = QTime::currentTime();
  stTime.start();
  abcPrune(turn, me, -666, 666, 666, &a, &b, &c, &move);

  qDebug() <<
    "face:" << FACE(hands[me][move]) <<
    "suit:" << SUIT(hands[me][move])+1 <<
    "move:" << move <<
    "turn:" << turn <<
    "moves:" << crdLeft <<
    "trump:" << trumpSuit <<
    "iters:" << gIterations <<
    "";

/*
  for (int h = 0; h < 3; h++) {
    fprintf(stderr, (h == me)?"*":" ");
    fprintf(stderr, "hand %i:", h);
    for (int f = 0; f < 10; f++) {
      if (hands[h][f]) {
        fprintf(stderr, " %2i.%i(%3i)", FACE(hands[h][f]), SUIT(hands[h][f]), hands[h][f]);
      } else {
        fprintf(stderr, " %2i.%i(%3i)", 0, 0, hands[h][f]);
      }
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "desk:");
  for (int f = 0; f < turn; f++) {
    fprintf(stderr, " %2i.%i(%3i)", FACE(desk[f]), SUIT(desk[f]), desk[f]);
  }
  fprintf(stderr, "\n");
*/
  if (move < 0) {
    fprintf(stderr, "fuck!\n");
    //goto again;
    abort();
  }

  Card *moveCard = newCard(FACE(hands[me][move]), SUIT(hands[me][move])+1);

  qDebug() << "move:" << moveCard->toString();

  mCards.remove(moveCard);
  mCardsOut.insert(moveCard);

  return moveCard;
}


///////////////////////////////////////////////////////////////////////////////
CheatPlayer::CheatPlayer (int aMyNumber, bool iStart, DeskView *aDeskView) : AiPlayer(aMyNumber, iStart, aDeskView) {
  mInvisibleHand = false;
}


/*
CheatPlayer &CheatPlayer::operator = (const Player &pl) {
  Player::clone(&pl);
  mInvisibleHand = false;
  return *this;
}


CheatPlayer &CheatPlayer::operator = (const CheatPlayer &pl) {
  Player::clone(&pl);
  if (allowDebugLog) mInvisibleHand = false;
  return *this;
}
*/
