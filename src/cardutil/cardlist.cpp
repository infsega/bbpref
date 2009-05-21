#include "cardlist.h"
#include "card.h"

//----------------------------------------------------------------------
TCardList::TCardList(int _Limit) : Tclist(_Limit) {

}


TCardList::~TCardList () {
  for (int f = aLimit-1; f >= 0; f--) {
    TCard *item = (TCard *)items[f];
    if (item) delete item;
  }
}


//----------------------------------------------------------------------
TCard * TCardList::Exist( int PackedName ) {
  int CName = PackedName/10,CMast;
  CMast = PackedName - CName*10;
  return Exist(CName,CMast);
}
//----------------------------------------------------------------------
TCard * TCardList::Exist( int _CName, int _CMast ) {
  TCard *Card=NULL;
  for (int i = 0; i < aLimit; i++) {
        Card = (TCard *) At(i);
        if ( Card && (Card -> CName == _CName && Card -> CMast== _CMast))
             return Card;
  }
  return NULL;
}
//----------------------------------------------------------------------
TCard * TCardList::MinCard( void ) {
  TCard *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
      tmp2 = (TCard *)this -> items[i];
      if (tmp2) {
      if (first) {
        first = !first;
        tmp1 = tmp2;
      } else {
        if (tmp1->CName > tmp2->CName)
          tmp1= tmp2;
      }
      }
  }
  if (!first)   return tmp1;
  return NULL;
}
//----------------------------------------------------------------------
TCard * TCardList::MaxCard( void ) {
  TCard *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
      tmp2 = (TCard *)this -> items[i];
      if (tmp2) {
         if (first) {
            first = !first;
            tmp1 = tmp2;
         } else {
           if (tmp1->CName > tmp2->CName)
          tmp1= tmp2;
         }
      }
  }
  if (!first)   return tmp1;
  return NULL;
}
//----------------------------------------------------------------------
TCard * TCardList::MinCard( int _CMast ) {
  TCard *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
      tmp2 = (TCard *)this -> items[i];
      if (tmp2!=NULL && tmp2->CMast==_CMast) {
        if (first) {
          first = !first;
          tmp1 = tmp2;
        } else {
          if (tmp1->CName > tmp2->CName)
            tmp1= tmp2;
        }
      }
  }
  if (!first)   return tmp1;
  return NULL;
}
//----------------------------------------------------------------------
TCard * TCardList::MaxCard( int _CMast ) {
  TCard *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
    tmp2 = (TCard *)this->items[i];
    if (tmp2!=NULL && tmp2->CMast==_CMast) {
      if (first) {
        first = !first;
        tmp1 = tmp2;
      } else {
        if (tmp1->CName < tmp2->CName)
          tmp1= tmp2;
      }
     }
  }
  if (!first)   return tmp1;
  return NULL;
}
//----------------------------------------------------------------------
int TCardList::AllCard( int _CMast ) {
  int j=0;
  for (int i = 0; i < aLimit; i++) {
      TCard *Card = (TCard *) this->items[i];
      if (Card && Card->CMast==_CMast) {
        j++;
      }
  }
  return j;
}
//----------------------------------------------------------------------
int TCardList::AllCard( void) {
  int j=0;
  for (int i = 0; i < aLimit; i++) {
      TCard *Card = (TCard *) this->items[i];
      if ( Card )
        j++;
  }
  return j;
}


void TCardList::mySort () {
  void **newi = new void *[aLimit];
  // count, copy and compress
  int cnt = 0;
  // copy
  for (int f = 0; f < aLimit; f++) {
    if (items[f]) newi[cnt++] = items[f];
  }
  // clear
  for (int f = cnt; f < aLimit; f++) newi[f] = 0;
  delete items;
  items = newi; aCount = cnt;
  // bubble sort
  for (int f = 0; f < cnt; f++) {
    for (int c = cnt-1; c > f; c--) {
      TCard *cf, *cc;
      cf = (TCard *)items[f];
      cc = (TCard *)items[c];
      int r = (cf->CMast)-(cc->CMast);
      if (!r) r = (cc->CName)-(cf->CName);
      if (r > 0) {
        // swap
        void *t = items[f];
        items[f] = items[c];
        items[c] = t;
      }
    }
  }
/*
  int j=0;
  int m;
  TCard *c1=NULL;
  void **anewitems = new void *[aLimit];
  for  ( int i=0;i<aLimit;i++ )
    anewitems[i]= NULL;
  for ( m=1;m<=3;m=m+2) {
    for (int c=FACE_ACE;c>=7;c--) {
      if ( (c1 = Exist(c,m))!=NULL ) {
        anewitems[j++] = c1;
        //items[IndexOf(c1)] = NULL;
      }
    }
  }

  for (m=2;m<=4;m=m+2) {
    for (int c=FACE_ACE;c>=7;c--) {
      if ( (c1 = Exist(c,m))!=NULL ) {
        anewitems[j++] = c1;
        //items[IndexOf(c1)] = NULL;
      }
    }
  }

  delete items;
  items = anewitems;
*/
/*  int j=0;
  TCard *c1=NULL;
  void **anewitems = new void *[aLimit];
  for  ( int i=0;i<aLimit;i++ )
    anewitems[i]= NULL;
  for (int m=1;m<=4;m++) {
    for (int c=7;c<=FACE_ACE;c++) {
      if ( (c1 = Exist(c,m))!=NULL ) {
        anewitems[j++] = c1;
//        items[IndexOf(c1)] = NULL;
      }
    }
  }
  delete items;
  items = anewitems;*/
}


TCard * TCardList::MoreThan( int _CName, int _CMast ) {
  if ( _CName==FACE_ACE ) return NULL;
  for(int i=_CName;i<=FACE_ACE;i++) {
    if(Exist(i,_CMast )) {
       return Exist(i,_CMast );
    }
  }
  return NULL;
}
//----------------------------------------------------------------------
TCard * TCardList::LessThan( int _CName, int _CMast ) {
  if ( _CName==7 ) return NULL;
  for(int i=_CName;i>=7;i--) {
    if(Exist(i,_CMast )) {
       return Exist(i,_CMast );
    }
  }
  return NULL;
}
//----------------------------------------------------------------------
TCard * TCardList::MoreThan( TCard *Card ) {
  if ( Card ) {
      if ( Card->CName==FACE_ACE ) return NULL;
      for(int i=Card->CName;i<=FACE_ACE;i++) {
        if(Exist(i,Card->CMast )) {
           return Exist(i,Card->CMast );
        }
      }
  }

  return NULL;
}
//----------------------------------------------------------------------
TCard * TCardList::LessThan( TCard *Card ) { //первая меньше чем переданная
  if ( Card ) {
      if (Card-> CName==7 ) return NULL;
      for(int i=Card-> CName;i>=7;i--) {
        if(Exist(i,Card-> CMast )) {
           return Exist(i,Card->CMast );
        }
      }
  }
  return NULL;
}
//----------------------------------------------------------------------
int TCardList::EmptyMast(int _CMast) {
  for ( int i = 1;i<=4;i++ ) {
    if ( i==_CMast ) continue; //k8:bug? break;
    if ( ! AllCard(_CMast) ) return i;
  }
  return 0;
}
//----------------------------------------------------------------------
TCard * TCardList::FirstCard(void) {
  return (TCard *)FirstItem();

}
//----------------------------------------------------------------------
TCard * TCardList::LastCard(void) {
  return (TCard *)LastItem();
}
//----------------------------------------------------------------------
TCard * TCardList::NextCard(void *PTCard) {
  return (TCard *) NextItem( IndexOf(PTCard) );
}
//----------------------------------------------------------------------
TCard * TCardList::NextCard(int _nIndex) {
  return (TCard *) NextItem(_nIndex);
}
//----------------------------------------------------------------------
void TCardList::AssignMast(Tclist *_oldlist,TMast Mast) { // Assign only selected mast from oldlist
  TCard *Card;
  for ( int i = 0;i < _oldlist -> aLimit;i++ )  {
    Card =(TCard* ) _oldlist ->At(i);
    if ( Card && Card -> CMast==Mast)  {
        Insert(Card);
    }
  }
}
//----------------------------------------------------------------------
