#include <QVector>

#include "card.h"


//---------------------------------------------------------------------------
void Card::SetCard( int _CName, int _CMast )
{
        CName=_CName;
        CMast=_CMast;

}
//---------------------------------------------------------------------------
Card::~Card() {
}

//---------------------------------------------------------------------------
Card::Card( int _CName, int _CMast ) {
        CName=_CName;
        CMast=_CMast;
}
//---------------------------------------------------------------------------
int operator > (Card &c1,Card &c2) {
          if (c1.CMast == c2.CMast && c1.CName > c2.CName) return 1;
          //if (c1.CMast == c2.CMast && c1.CName < c2.CName) return -1;
          return 0;
         };
//---------------------------------------------------------------------------
int operator < (Card &c1,Card &c2) {
          if (c1.CMast == c2.CMast && c1.CName < c2.CName) return 1;
          //if (c1.CMast == c2.CMast && c1.CName > c2.CName) return -1;
          return 0;
         };
