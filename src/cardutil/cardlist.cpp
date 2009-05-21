#include "cardlist.h"
#include "card.h"

//----------------------------------------------------------------------
TCardList::TCardList(int _Limit) : Tclist(_Limit) {

}


TCardList::~TCardList () {
  for (int f = aLimit-1; f >= 0; f--) {
    Card *item = (Card *)items[f];
    if (item) delete item;
  }
}


//----------------------------------------------------------------------
Card * TCardList::Exist( int PackedName ) {
  int CName = PackedName/10,CMast;
  CMast = PackedName - CName*10;
  return Exist(CName,CMast);
}
//----------------------------------------------------------------------
Card * TCardList::Exist( int _CName, int _CMast ) {
  Card *card=NULL;
  for (int i = 0; i < aLimit; i++) {
        card = (Card *) At(i);
        if ( card && (card -> CName == _CName && card -> CMast== _CMast))
             return card;
  }
  return NULL;
}
//----------------------------------------------------------------------
Card * TCardList::MinCard( void ) {
  Card *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
      tmp2 = (Card *)this -> items[i];
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
Card * TCardList::MaxCard( void ) {
  Card *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
      tmp2 = (Card *)this -> items[i];
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
Card * TCardList::MinCard( int _CMast ) {
  Card *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
      tmp2 = (Card *)this -> items[i];
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
Card * TCardList::MaxCard( int _CMast ) {
  Card *tmp1=NULL,*tmp2=NULL;
  char first=1;
  for (int i = 0; i < aLimit; i++) {
    tmp2 = (Card *)this->items[i];
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
      Card *card = (Card *) this->items[i];
      if (card && card->CMast==_CMast) {
        j++;
      }
  }
  return j;
}
//----------------------------------------------------------------------
int TCardList::AllCard( void) {
  int j=0;
  for (int i = 0; i < aLimit; i++) {
      Card *card = (Card *) this->items[i];
      if ( card )
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
      Card *cf, *cc;
      cf = (Card *)items[f];
      cc = (Card *)items[c];
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
  Card *c1=NULL;
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
  Card *c1=NULL;
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


Card * TCardList::MoreThan( int _CName, int _CMast ) {
  if ( _CName==FACE_ACE ) return NULL;
  for(int i=_CName;i<=FACE_ACE;i++) {
    if(Exist(i,_CMast )) {
       return Exist(i,_CMast );
    }
  }
  return NULL;
}
//----------------------------------------------------------------------
Card * TCardList::LessThan( int _CName, int _CMast ) {
  if ( _CName==7 ) return NULL;
  for(int i=_CName;i>=7;i--) {
    if(Exist(i,_CMast )) {
       return Exist(i,_CMast );
    }
  }
  return NULL;
}
//----------------------------------------------------------------------
Card * TCardList::MoreThan( Card *card ) {
  if ( card ) {
      if ( card->CName==FACE_ACE ) return NULL;
      for(int i=card->CName;i<=FACE_ACE;i++) {
        if(Exist(i,card->CMast )) {
           return Exist(i,card->CMast );
        }
      }
  }

  return NULL;
}
//----------------------------------------------------------------------
Card * TCardList::LessThan( Card *card ) { //первая меньше чем переданная
  if ( card ) {
      if (card-> CName==7 ) return NULL;
      for(int i=card-> CName;i>=7;i--) {
        if(Exist(i,card-> CMast )) {
           return Exist(i,card->CMast );
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
Card * TCardList::FirstCard(void) {
  return (Card *)FirstItem();

}
//----------------------------------------------------------------------
Card * TCardList::LastCard(void) {
  return (Card *)LastItem();
}
//----------------------------------------------------------------------
Card * TCardList::NextCard(void *PTCard) {
  return (Card *) NextItem( IndexOf(PTCard) );
}
//----------------------------------------------------------------------
Card * TCardList::NextCard(int _nIndex) {
  return (Card *) NextItem(_nIndex);
}
//----------------------------------------------------------------------
void TCardList::AssignMast(Tclist *_oldlist,TMast Mast) { // Assign only selected mast from oldlist
  Card *card;
  for ( int i = 0;i < _oldlist -> aLimit;i++ )  {
    card =(Card* ) _oldlist ->At(i);
    if ( card && card -> CMast==Mast)  {
        Insert(card);
    }
  }
}
//----------------------------------------------------------------------
