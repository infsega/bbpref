#include <time.h>

#include <QtCore>

#include "card.h"
#include "deck.h"
#include "prfconst.h"


Deck::Deck () : CardList() {
  for (int suit = 1; suit <= 4; suit++) {
    for (int face = 7; face <= FACE_ACE; face++) {
      mList << new Card(face, suit);
    }
  }
}


void Deck::clear () {
  foreach (Card *c, mList) if (c) delete c;
  mList.clear();
}


void Deck::shuffle () {
  removeNulls();
  time_t t;
  qsrand((unsigned)time(&t));
  //k8: Fisher-Yates shuffler
  for (int f = mList.size()-1; f >= 0; f--) {
    int n = (qrand()/256)%(f+1); // 0<=n<=f
    mList.swap(f, n);
  }
}
