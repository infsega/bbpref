#ifndef HUMAN_G_H
#define HUMAN_G_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#include "gamer.h"


class THuman :public TGamer {
public:
  THuman (int _nGamer);
  THuman (int _nGamer, TDeskView *_DeskView);
  TGamesType makeout4game (); // после сноса чего играем
  TGamesType makeout4miser ();
  TCard *makemove (TCard *lMove, TCard *rMove, TGamer *aLeftGamer, TGamer *aRightGamer); //ход
  TGamesType makemove (TGamesType lMove, TGamesType rMove); //ход при торговле
  TGamesType makemove (TGamesType MaxGame, int HaveAVist, int nGamerVist); // после получения игроком прикупа - пасс или вист
  void HintCard (int lx, int ly);
};


#endif
