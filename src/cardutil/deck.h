//---------------------------------------------------------------------------
#ifndef DECK_H
#define DECK_H

#include "card.h"
#include "cardlist.h"


// deck is owns the cards
class Deck : public CardList {
public:
  Deck ();

  void shuffle ();
};


#endif
