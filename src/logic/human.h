#ifndef HUMAN_H
#define HUMAN_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#include "player.h"


class HumanPlayer : public Player {
public:
  HumanPlayer (int aMyNumber, DeskView *aDeskView=0);

  eGameBid dropForGame (); // после сноса чего играем
  eGameBid dropForMisere ();

  Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer); //ход
  eGameBid moveBidding (eGameBid lMove, eGameBid rMove); //ход при торговле
  eGameBid moveFinalBid (eGameBid MaxGame, int HaveAVist, int nGamerVist); // после получения игроком прикупа - пасс или вист

  virtual void hilightCard (int lx, int ly);
  virtual void clear ();

  HumanPlayer &operator = (const Player &pl);
  HumanPlayer &operator = (const HumanPlayer &pl);
};


#endif
