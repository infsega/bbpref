#include <QVector>

#include "card.h"


//---------------------------------------------------------------------------
void TCard::SetCard( int _CName, int _CMast )
{
        CName=_CName;
        CMast=_CMast;

}
//---------------------------------------------------------------------------
TCard::~TCard() {
}

//---------------------------------------------------------------------------
TCard::TCard( int _CName, int _CMast ) {
        CName=_CName;
        CMast=_CMast;
}
//---------------------------------------------------------------------------
int operator > (TCard &c1,TCard &c2) {
          if (c1.CMast == c2.CMast && c1.CName > c2.CName) return 1;
          //if (c1.CMast == c2.CMast && c1.CName < c2.CName) return -1;
          return 0;
         };
//---------------------------------------------------------------------------
int operator < (TCard &c1,TCard &c2) {
          if (c1.CMast == c2.CMast && c1.CName < c2.CName) return 1;
          //if (c1.CMast == c2.CMast && c1.CName > c2.CName) return -1;
          return 0;
         };
