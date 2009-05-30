#ifndef AIALPHABETA_H
#define AIALPHABETA_H

#include "aiplayer.h"
#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#include "player.h"


class CheatPlayer : public AiPlayer {
public:
  CheatPlayer (int aMyNumber, DeskView *aDeskView=0);

  Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer, bool isPassOut); //ход

  virtual bool invisibleHand ();
};


#endif
