//---------------------------------------------------------------------------
#include <time.h>
#include <stdlib.h> //for random numbers
#include "card.h"
#include "coloda.h"
#include "prfconst.h"
//---------------------------------------------------------------------------
TColoda::TColoda(int _Limit) : TCardList(_Limit) {
        for (int m=1;m<=4;m++) {
                for (int k=7;k<=A;k++) {
                  TCard *nc = new TCard(k,m);
                  //nc->SetImage();
                  Insert(nc);
                }
        }
//        Mix();
//        Insert(new TCard(10,10));
}
//---------------------------------------------------------------------------
void TColoda::Mix(void)
{
   int i;
   time_t t;
   srand((unsigned) time(&t));
   for ( int j=0;j<=3;j++ ) {
       i = rand() % 1234;
       while(i--)
            Swap(rand() % CARDINCOLODA,rand() % CARDINCOLODA);
   }
}
//---------------------------------------------------------------------------
