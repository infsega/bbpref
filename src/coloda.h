//---------------------------------------------------------------------------
#ifndef DECK_H
#define DECK_H

#include "card.h"
#include "cardlist.h"


class  TColoda : public TCardList {
public:
  TColoda (int _Limit);
  void Mix ();
};


#endif
