#include <time.h>

#include <QtCore>

#include "card.h"
#include "deck.h"
#include "prfconst.h"


Deck::Deck (int _Limit) : CardList(_Limit) {
  for (int m = 1; m <= 4; m++) {
    for (int k = 7; k <= FACE_ACE; k++) {
      Card *nc = new Card(k, m);
      Insert(nc);
    }
  }
}


void Deck::shuffle () {
/*
   int i;
   time_t t;
   srand((unsigned) time(&t));
   for ( int j=0;j<=3;j++ ) {
       i = rand() % 1234;
       while(i--)
            Swap(rand() % CARDINCOLODA,rand() % CARDINCOLODA);
   }
*/
  time_t t;
  qsrand((unsigned)time(&t));
  //k8: Fisher-Yates shuffler
  for (int f = aCount-1; f >= 0; f--) {
    int n = (qrand()/256)%(f+1); // 0<=n<=f
    Swap(f, n);
  }
}
