#ifndef AIALPHABETA_H
#define AIALPHABETA_H

#include "prfconst.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "deskview.h"
#include "player.h"


class CheatPlayer : public Player {
public:
  CheatPlayer (int aMyNumber, DeskView *aDeskView=0);

  Card *moveSelectCard (Card *lMove, Card *rMove, Player *aLeftPlayer, Player *aRightPlayer); //ход

  CheatPlayer &operator = (const Player &pl);
  CheatPlayer &operator = (const CheatPlayer &pl);

  virtual bool isInvisibleHand ();
};


#endif
