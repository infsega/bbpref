//---------------------------------------------------------------------------
#ifndef DECK_H
#define DECK_H

#include "card.h"
#include "cardlist.h"


class  Deck : public CardList {
public:
  Deck (int _Limit);
  void shuffle ();
};


#endif
