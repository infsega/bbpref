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
static int gPassOutSuit; // нужная масть для первого или второго круга распасов
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
 * карты должны быть отсортированы по мастям в порядке убывания "морды"
 * a: игрок player набрал максимум вот столько
 * b: игрок player+1 набрал максимум вот столько
 * c: игрок player+2 набрал максимум вот столько
 * возврат: то же самое
 *
 * идея и псевдокод взяты отсюда: http://clauchau.free.fr/gamma.html
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
      // первый ход может быть любой ваще, если это не первый и не второй круг распасов
      if (gPassOutSuit >= 0 && crdSuit != gPassOutSuit && hand->suitCount[gPassOutSuit]) {
        // не, это очень херовая масть, начнём с верной масти
        tmp = hand->suitStart[gPassOutSuit];
        if (tmp == crdNo) abort(); // а такого не бывает
        if (tmp < crdNo) break; // ну нет у нас такой, и уже всё, что было, проверили
        // скипаем и повторяем выборы
        crdNo = tmp;
        continue;
      }
      goto doMove;
    }
    // check for valid move
    // выход в правильную масть?
    if (crdSuit == xDeskSuits[0]) goto doMove;
    // не, не та масть; ну-ка, чо у нас на руках ваще?
    // нужная масть у нас есть?
    if (hand->suitCount[xDeskSuits[0]]) {
      // таки есть, потому это очень хуёвый вариант; ходим сразу с нужной масти
      tmp = hand->suitStart[xDeskSuits[0]];
      if (tmp < crdNo) break; // всё, нечего больше искать
      if (tmp > crdNo) {
        // скипаем
        crdNo = tmp;
        continue;
      }
      // вот этой и ходим
      goto doMove;
    }
    // не, нужной масти нет
    // а козырь есть?
    if (gTrumpSuit <= 3) {
      // игра козырная, есть козыри?
      if (hand->suitCount[gTrumpSuit]) {
        // таки есть
        tmp = hand->suitStart[gTrumpSuit];
        if (tmp < crdNo) break; // всё, нечего больше искать
        if (tmp > crdNo) {
          // скипаем
          crdNo = tmp;
          continue;
        }
        // вот этой и ходим
        goto doMove;
      } else {
        // не, и козырей нет, можно кидать чо попало
        goto doMove;
      }
    } else {
      // игра бескозырная, тут любая карта пойдёт, хули
      goto doMove;
    }
doMove:
/*
    movesChecked++;
    if (firstm < 0) firstm = crdNo;
*/
    // проскипаем последовательность из плавно убывающих карт одной масти
    // очевидно, что при таком раскладе похуй, какой из них ходить
    int scnt = hand->suitCount[crdSuit];
    if (scnt > 1) {
      // в этой масти есть ещё карты
      // проверим, есть ли у кого ещё эта масть
      if (xHands[0].suitCount[crdSuit]+xHands[1].suitCount[crdSuit]+xHands[2].suitCount[crdSuit] <= scnt) {
        // единственный гордый владелец этой масти; пробуем только одну её карту
        int tsuit = crdSuit+1;
        while (tsuit <= 3 && hand->suitCount[tsuit] == 0) tsuit++;
        crdNext = tsuit>3 ? 11 : hand->suitStart[tsuit];
      } else {
        // такая масть есть ещё у кого-то
        int tface = crdFace+1;
        while (crdNext <= 10 && hand->suits[crdNext] == crdSuit && hand->faces[crdNext] == tface) {
          crdNext++;
          tface++;
        }
      }
    }
    // кидаем карту на стол
    xDeskSuits[turn] = crdSuit;
    xDeskFaces[turn] = crdFace;
    // убираем карту из руки
    hand->suitCount[crdSuit]--;
    if (crdNo == hand->suitStart[crdSuit]) hand->suitStart[crdSuit]++;
    hand->faces[crdNo] = 0;

    //lastMoveF = crdFace; lastMoveS = crdSuit;
    int y, z, x;
    if (turn == 2) {
      // the turn is done, count tricks
      //who = whoTakes(pdesk, gTrumpSuit);
      // а кто, собственно, забрал?
      if (gTrumpSuit <= 3) {
        // trump game
        if (xDeskSuits[0] == gTrumpSuit) {
          // нулевой козырнул
          who = 0; tmp = xDeskFaces[0];
          if (xDeskSuits[1] == xDeskSuits[0] && xDeskFaces[1] > tmp) { tmp = xDeskFaces[1]; who = 1; }
          if (xDeskSuits[2] == xDeskSuits[0] && xDeskFaces[2] > tmp) who = 2;
        } else if (xDeskSuits[1] == gTrumpSuit) {
          // первый козырнул
          who = 1; tmp = xDeskFaces[0];
          if (xDeskSuits[2] == xDeskSuits[1] && xDeskFaces[2] > tmp) who = 2;
        } else if (xDeskSuits[2] == gTrumpSuit) {
          // второй козырнул
          who = 2;
        } else {
          // никто не козырял
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
      xHands[who].tricks++; // прибавили взятку
#ifdef ABDEBUG
      printf("==%i takes; cards left: %i; turn: %i\n", who, xCardsLeft, turn);
      printStatus(turn, player, 1);
#endif
      if (xCardsLeft < 0) abort();
      if (!xCardsLeft) {
        // всё, отбомбились, даёшь коэффициенты
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
          // я играю; выиграл ли?
          if (x < gGameBid) {
            // нет, обезлаплен
            x = -gGameBid-1; /*y = z = 666;*/
          } else {
            // да, взял своё
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
          // я вистую; получилось ли?
          if (xHands[gWhoPlays].tricks < gGameBid) {
            // по любому засадили чувачка
            //x = 666-(gGameBid-xHands[gWhoPlays].tricks); // на сколько
            // чувак в жопе
            if (gWhoPlays == newPlayer) {
              //y = -xHands[gWhoPlays].tricks-1;
              //z = 666;
            } else {
              //z = -xHands[gWhoPlays].tricks-1;
              //y = 666;
            }
          } else {
            // нет, чувак, увы, взял своё; а я?
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
        // рекурсивно проверяем дальше
        //abcPrune(newTurn, newPlayer, -c, -a, b, &y, &z, &x, NULL);
        if (who == player) {
          // я же и забрал, снова здорово
          abcPrune(0, player, a, b, c, &x, &y, &z, NULL);
        } else if (who == newPlayer) {
          // следующий забрал; красота и благолепие
          abcPrune(0, newPlayer, -c, -a, b, &y, &z, &x, NULL);
        } else {
          // предыдущий забрал; вот такие вот параметры вышли; путём трэйсинга, да
          abcPrune(0, who, -b, c, -a, &z, &x, &y, NULL);
        }
      }
      // брали взятку? восстановим статус кво
      xHands[who].tricks--;
    } else {
      // рекурсивно проверяем дальше
      abcPrune(newTurn, newPlayer, -c, -a, b, &y, &z, &x, NULL);
    }
    // восстановим стол
    xDeskFaces[0] = sDeskFaces[0]; xDeskFaces[1] = sDeskFaces[1]; xDeskFaces[2] = sDeskFaces[2];
    xDeskSuits[0] = sDeskSuits[0]; xDeskSuits[1] = sDeskSuits[1]; xDeskSuits[2] = sDeskSuits[2];
    // вернём в руку карту
    hand->suitCount[crdSuit]++;
    hand->faces[crdNo] = crdFace;
    if (crdNo+1 == hand->suitStart[crdSuit]) hand->suitStart[crdSuit]--;
    // проверим, чо нашли
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
      if (x > b || x > c) break; // всё, дальше искать не надо, всё равно мы крутые; goto done;
      if (x > a) a = x;
    }
    // берём следующую карту
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
  printf ("CheatPlayer (%d) moves\n", mPlayerNo);
  
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


  if (!lMove && !rMove && crdLeft == 10) { 
    return AiPlayer::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer, isPassOut);
  }


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
    // это распасы, первый или второй круг, первый ход
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

  // оптимизации
/*
  if (turn > 0) {
    // можем вообще взять?
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
CheatPlayer::CheatPlayer (int aMyNumber, DeskView *aDeskView) : AiPlayer(aMyNumber, aDeskView) {
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
