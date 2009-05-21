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
  tGameBid makeout4game (); // после сноса чего играем
  tGameBid makeout4miser ();
  TCard *makemove (TCard *lMove, TCard *rMove, Player *aLeftGamer, Player *aRightGamer); //ход
  tGameBid makemove (tGameBid lMove, tGameBid rMove); //ход при торговле
  tGameBid makemove (tGameBid MaxGame, int HaveAVist, int nGamerVist); // после получения игроком прикупа - пасс или вист
  void HintCard (int lx, int ly);
};


#endif
