#include <QDebug>

#include <stdlib.h>
#include <stdio.h>

#include "prfconst.h"

#include "formbid.h"
#include "kpref.h"


#include "deskview.h"
#include "player.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "aialphabeta.h"


// 1-33; 0: unused
#define SUIT(c)  (((c)-1)%4)
#define FACE(c)  ((((c)-1)/4)+7)
#define CARD(face,suit)  (((face-7)*4+(suit))+1)

typedef unsigned char  card_t;


static inline int hasSuit1 (card_t *hand, int suit) {
  int f;
  for (f = 9; f >= 0; f--) {
    card_t c = *hand++;
    if (c && SUIT(c) == suit) return 1;
  }
  return 0;
}


/* !0: the move is valid */
/* trumpSuit: -1 -- NT */
static inline int isValidMove1 (card_t *hand, card_t card, card_t firstOnDesk, int trumpSuit) {
  int cs = SUIT(card), ps = SUIT(firstOnDesk);
  if (cs == ps) return 1; // same suits: valid
  // different suits
  if (hasSuit1(hand, ps)) {
    return 0; // has the necessary suit, invalid
  }
  // no required suit
  if (trumpSuit >= 0 && trumpSuit <= 3) {
    // trump play
    if (cs == trumpSuit) return 1; // trump; valid
    if (hasSuit1(hand, trumpSuit)) {
      return 0; // has trump: invalid
    }
    // has no trumps, valid
  }
  // no trumps, no necessary suit: anything is valid
  return 1;
}


// idiotic code!
// find who takes the trick (0-2)
static inline int whoTakes (card_t *desk, int trumpSuit) {
  card_t chk[3], stt[3];
  int f;
  //if (trumpSuit < 0) trumpSuit = 255;
  chk[0] = FACE(desk[0]); chk[1] = FACE(desk[1]); chk[2] = FACE(desk[2]);
  stt[0] = SUIT(desk[0]); stt[1] = SUIT(desk[1]); stt[2] = SUIT(desk[2]);
  // if no trump, force trump to be the 1st card's suit
  if (stt[0] != trumpSuit && stt[1] != trumpSuit && stt[2] != trumpSuit) trumpSuit = stt[0];
  int res = -1, fc = 0;
  for (f = 2; f >= 0; f--) {
    if (stt[f] == trumpSuit) {
      int face = FACE(desk[f]);
      if (face > fc) { res = f; fc = face; }
    }
  }
  return res;
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
//static int gGameBid; // >=6
//static int gMoves[4][10];
static int gTricks[3];
//static int gE0, gE1, gMe;
//static int gWhoMoves;
static int gIterations;

static int gTrk = 0;
static bool gPassOut;


static void abcPrune (
  int turn,
  card_t *desk,
  card_t *h0, card_t *h1, card_t *h2,
  int a, int b, int c,
  int *ra, int *rb, int *rc, int *rm,
  int depth,
  int first)
{
#ifdef XXDEBUG
  dlogf(">depth: %i; turn: %i\n", depth, turn);
  dbgPrintCards(h0, 10, " h0: ");
  dbgPrintCards(h1, 10, " h1: ");
  dbgPrintCards(h2, 10, " h2: ");
#endif
  if (depth < 1) {
    gIterations++;
    if (gPassOut) {
      *ra = gTricks[turn];
      *rb = gTricks[(turn+1)%3];
      *rc = gTricks[(turn+2)%3];
    } else {
      *ra = 10-gTricks[turn];
      *rb = 10-gTricks[(turn+1)%3];
      *rc = 10-gTricks[(turn+2)%3];
    }
/*
    switch (gWhoMoves) {
      case 0:
        if (*ra < gGameBid) *ra = -20; // loss
        else if (*ra >= gGameBid) *ra = 20; // win
        break;
      case 1:
        if (*rb < gGameBid) *rb = -20; // loss
        else if (*rb >= gGameBid) *rb = 20; // win
        break;
      case 2:
        if (*rc < gGameBid) *rc = -20; // loss
        else if (*rc >= gGameBid) *rc = 20; // win
        break;
    }
*/
#ifdef XXDEBUG
    dlogf(" depth0: %i %i %i\n", gTricks[0], gTricks[1], gTricks[2]);
#endif
    return;
/*
      (x, y, z) := some static additive evaluation of Position,
      x measuring how good is Position for the Player to move,
      y measuring how good is Position for the next Player,
      z measuring how good is Position for the further next Player,
      the higher the better, -infinite for a defeat, +infinite for a win;

      if the game is over then return (x, y, z, "game over");
      else return ( min(x-y, x-z), min(y-x, y-z), min(z-x, z-y), "static" );
*/
  }
  int bestx = -20;
  int worsty = 20, worstz = 20;
  int bestm = -1;
  int n = 0; // will count equivalent moves
  int crdNo, crd, tmp, who = -1;
  card_t pdesk[3];
  int newDepth = depth-(turn==2?1:0), newTurn = (turn+1)%3;
  pdesk[0] = desk[0]; pdesk[1] = desk[1]; pdesk[2] = desk[2];
  for (crdNo = 0; crdNo < 10; crdNo++) {
    crd = h0[crdNo]; if (!crd) continue;
    h0[crdNo] = 0;
    if (!turn || isValidMove1(h0, crd, pdesk[0], gTrumpSuit)) {
      if (bestm < 0) bestm = crdNo;
      pdesk[turn] = crd;
      if (turn == 2) {
        // the turn is done, count tricks
        who = whoTakes(pdesk, gTrumpSuit);
        gTricks[who]++;
#ifdef XXDEBUG
        static char buf[256], buf1[128];
        sprintf(buf, "desk: "); sprintCards(buf, pdesk, 3);
        sprintf(buf1, "  %i takes (depth:%i)", who, depth);
        dlogf("%s%s\n", buf, buf1);
#endif
      } else {
        who = -1;
#ifdef XXDEBUG
        static char buf[256], buf1[128];
        sprintf(buf, " desk: "); sprintCards(buf, pdesk, turn+1);
        sprintf(buf1, "  plr:%i (depth:%i)", (turn+gWhoMoves)%3, depth);
        dlogf("%s%s\n", buf, buf1);
#endif
      }
      int y, z, x;
      abcPrune(newTurn, pdesk, h1, h2, h0, -c, -a, b, &y, &z, &x, &tmp, newDepth, 0);
      if (first && x > bestx) {
        gTrk = gTricks[turn];
      }
      if (who >= 0) gTricks[who]--;
      h0[crdNo] = crd;
      if (x == bestx) {
        // we've found an equivalent move
        n++;
        if (y < worsty) worsty = y;
        if (z < worstz) worstz = z;
        //if (myrand()%n == n-1) bestm = crdNo;
        // hands are sorted, so take the smallest possible card
        bestm = crdNo;
      } else if (x > bestx) {
        // we've found a better move
        n = 1;
        bestm = crdNo;
        bestx = x; worsty = y; worstz = z;
        if (x > b || x > c) goto done;
        if (x > a) a = x;
      }
    } else h0[crdNo] = crd;
  }
done:
  *ra = bestx; *rb = worsty; *rc = worstz; *rm = bestm;
#ifdef XXDEBUG
  dlogf("<depth: %i; turn: %i\n", depth, turn);
#endif
}


static card_t hands[3][10];


/*
 * aLeftPlayer: next in turn
 * aRightPlayer: prev in turn
 * 0, 1th
 * 1th, 2nd
 */
Card *CheatPlayer::moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer) {
  card_t hds[3][10], hands[3][10];
  card_t desk[3];
  int me = mPlayerNo-1;
  int leftP = (me+1)%3;
  int rightP = (me+2)%3;
  int crdLeft = 0;
  int trumpSuit = 0;
  int tricks[3];
  Player *movePlrObj = 0;

  // build hands
  for (int c = 0; c < 3; c++) {
    int hNo = 0;
    CardList *clst = 0;
    switch (c) {
      case 0: hNo = me; clst = &mCards; break;
      case 1: hNo = leftP; clst = &(aLeftPlayer->mCards); break;
      case 2: hNo = rightP; clst = &(aRightPlayer->mCards); break;
      default: break;
    }
    for (int f = 0; f < 10; f++) hds[hNo][f] = hands[hNo][f] = 0;
    int pos = 0;
    for (int f = 0; f < clst->size(); f++) {
      Card *ct = clst->at(f);
      if (!ct) continue;
      hands[hNo][pos++] = CARD(ct->face(), ct->suit()-1);
      if (pos > crdLeft) crdLeft = pos;
    }
    xsortCards(hands[hNo], pos);
  }

  if (!lMove && !rMove && crdLeft == 10) {
    return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
  }

  // find game and gameplr
  if (mMyGame == gtPass || mMyGame == vist) {
    eGameBid b = aLeftPlayer->mMyGame;
    if (b == gtPass || b == vist) {
      b = aRightPlayer->mMyGame;
      movePlrObj = aRightPlayer;
    } else movePlrObj = aLeftPlayer;
/*
    if (b == g86catch || mMyGame == g86 || mMyGame == raspass) {
      return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
    }
*/
    gPassOut = (b == g86 || b == g86catch || b == raspass);
    trumpSuit = (b-(b/10)*10)-1;
  } else {
    trumpSuit = (mMyGame-(mMyGame/10)*10)-1;
    movePlrObj = this;
    gPassOut = (mMyGame == g86 || mMyGame == g86catch || mMyGame == raspass);
  }

  // build desk
  int turn = 0;
  if (lMove) {
    desk[turn++] = CARD(lMove->face(), lMove->suit()-1);
    if (rMove) desk[turn++] = CARD(rMove->face(), rMove->suit()-1);
  } else if (rMove) {
    desk[turn++] = CARD(rMove->face(), rMove->suit()-1);
  }

  // turn:
  //   0: me 1st
  //   1: rightP 1st
  //   2: leftP 1st
  int movePlrNo = me;
  switch (turn) {
    case 1:
      movePlrNo = rightP;
      tricks[0] = aRightPlayer->mTricksTaken;
      tricks[1] = this->mTricksTaken;
      tricks[2] = aLeftPlayer->mTricksTaken;
      break;
    case 2:
      movePlrNo = leftP;
      tricks[0] = aLeftPlayer->mTricksTaken;
      tricks[1] = aRightPlayer->mTricksTaken;
      tricks[2] = this->mTricksTaken;
      break;
    default:
      movePlrNo = me;
      tricks[0] = this->mTricksTaken;
      tricks[1] = aLeftPlayer->mTricksTaken;
      tricks[2] = aRightPlayer->mTricksTaken;
      //gWhoMoves = me;
      break;
  }
  for (int f = 0; f < 10; f++) {
    hds[0][f] = hands[movePlrNo][f];
    hds[1][f] = hands[(movePlrNo+1)%3][f];
    hds[2][f] = hands[(movePlrNo+2)%3][f];
  }
  //tricks[movePlrNo] = movePlrObj->mTricksTaken;

/*
  //if (mMyGame == gtPass || mMyGame == vist) return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
  if (mMyGame == g86catch) return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
  if (mMyGame == g86) return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
  //if (mMyGame == raspass) return Player::moveSelectCard(lMove, rMove, aLeftPlayer, aRightPlayer);
*/

  // search for move
  //gWhoMoves = me;
  gTrumpSuit = trumpSuit;
  //gGameBid = bid;

  int a, b, c, move = -1;
  //tricks[0] = tricks[1] = tricks[2] = 0;

  gTricks[0] = tricks[0]; gTricks[1] = tricks[1]; gTricks[2] = tricks[2];
  gIterations = 0;

  gTrk = 0;
  fprintf(stderr, "alphabeta: turn=%i (%i)\n", turn, crdLeft);
  //fprintf(stderr, "alphabeta: turn=%i (%i)\n", turn, crdLeft);

  abcPrune(turn, desk, hds[turn], hds[(turn+1)%3], hds[(turn+2)%3], -20, 20, 20, &a, &b, &c, &move, crdLeft, 1);
  if (move < 0 || move > 9 || !hds[turn][move]) {
    fprintf(stdout, "shitmove: %i; %i variants checked\n", move, gIterations);
    abort();
  }
  fprintf(stdout, "%i variants checked (%i)\n", gIterations, gTrk);
  //desk[turn] = hds[turn][move]; hds[turn][move] = 0;
/*
      if (turn == 2) {
        // take it
        int who = whoTakes(desk, gTrumpSuit);
        tricks[who]++;
      }
    }
  }
  tricks[0] = gTricks[0]; tricks[1] = gTricks[1]; tricks[2] = gTricks[2];
*/
  Card *moveCard = newCard(FACE(hds[turn][move]), SUIT(hds[turn][move])+1);

  qDebug() << "move:" << moveCard->toString();

  mCards.remove(moveCard);
  mCardsOut.insert(moveCard);

  return moveCard;
}


///////////////////////////////////////////////////////////////////////////////
CheatPlayer::CheatPlayer (int aMyNumber, DeskView *aDeskView) : Player(aMyNumber, aDeskView) {
  mInvisibleHand = false;
}


CheatPlayer &CheatPlayer::operator = (const Player &pl) {
  Player::clone(&pl);
  mInvisibleHand = false;
  return *this;
}


CheatPlayer &CheatPlayer::operator = (const CheatPlayer &pl) {
  Player::clone(&pl);
  mInvisibleHand = false;
  return *this;
}


bool CheatPlayer::isInvisibleHand () {
  return false;
}
