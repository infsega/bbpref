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
  HumanPlayer (int _nGamer);
  HumanPlayer (int _nGamer, TDeskView *_DeskView);
  eGameBid makeout4game (); // после сноса чего играем
  eGameBid makeout4miser ();
  Card *makemove (Card *lMove, Card *rMove, Player *aLeftGamer, Player *aRightGamer); //ход
  eGameBid makemove (eGameBid lMove, eGameBid rMove); //ход при торговле
  eGameBid makemove (eGameBid MaxGame, int HaveAVist, int nGamerVist); // после получения игроком прикупа - пасс или вист

  virtual void HintCard (int lx, int ly);
  virtual void clear ();
};


#endif
