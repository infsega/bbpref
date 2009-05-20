#include "card.h"
#define DEBUG
#ifdef DEBUG
#include <stdio.h>
#include "debug.h"
#endif

#include <QVector>


//---------------------------------------------------------------------------
void TCard::SetCard( int _CName, int _CMast )
{
        CName=_CName;
        CMast=_CMast;
          #ifdef DEBUG
              char msgbuff[1024];
              sprintf(msgbuff,"TCard created %p",this);
              debugprint(msgbuff);
          #endif

}
//---------------------------------------------------------------------------
TCard::~TCard() {
  #ifdef DEBUG
      char msgbuff[1024];
      sprintf(msgbuff,"TCard deleted %p",this);
      debugprint(msgbuff);
  #endif

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
//---------------------------------------------------------------------------
