//#define DEBUG
#include "prfconst.h"

#ifdef DEBUG
#include <stdio.h>
#include "debug.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "ctlist.h"

//--------------------------------------------------------------------------
Tclist::Tclist(int _Limit) {
  aCount = 0;
  aLimit = _Limit;
//  items =  malloc(_Limit * sizeof (void  *) );
  items = new void *[_Limit];
/*  #ifdef P4LIN
  for  ( int i=0;i<aLimit;i++ )
      items[i]= NULL;
  #endif*/
//  #ifdef P4WIN
    memset(items,0,sizeof(void *)*aLimit);
//  #endif
}
//--------------------------------------------------------------------------
Tclist::~Tclist() {
/*
  for( int i =  0; i < aLimit; i++ ) {
    void *item = At(i);
    if (item!=NULL) delete item;
  }
*/
  delete items;
//  free (items);
}
//--------------------------------------------------------------------------
/*void *Tclist::At(int _Index) {
  return _Index >= aLimit?NULL:items[_Index];
} */
//--------------------------------------------------------------------------
/*int Tclist::IndexOf(void *_Item) {
  for  ( int i=0;i<aLimit;i++ ) {
    if ( At(i)==_Item ) return i;
  }
  return -1;
} */
//--------------------------------------------------------------------------
int Tclist::Insert(void *_Item) {
  for  ( int i=0;i<aLimit;i++ ) {
    if ( At(i)==NULL ) {
      items[i]= _Item;
      aCount++;
      return i;
    }
  }
  return -1; //return NULL;
}
//--------------------------------------------------------------------------
void Tclist::AtPut(int _Index, void *_Item) {
  if ( _Index < aLimit )   {
      if ( items[_Index]==NULL ) aCount++; // ���� ������ �� ���� �� ������� ������� �� ����. ������� ����
      items[_Index]= _Item;
  }
}
//--------------------------------------------------------------------------
void Tclist::Free(void *_Item) {
  int index = IndexOf(_Item);
  if ( _Item ) {
    delete _Item;
    #ifdef DEBUG
       char msgbuff[1024];
       sprintf(msgbuff,"Tclist %p: Item Free %i %p",this,index,_Item);
       debugprint(msgbuff);
    #endif
  }
  items[index]=NULL;
  aCount--;
}
//--------------------------------------------------------------------------
void Tclist::Remove(void *_Item) {
  int index = IndexOf(_Item);
  items[index]=NULL;
  aCount--;
  #ifdef DEBUG
       char msgbuff[1024];
       sprintf(msgbuff,"Tclist %p: Item Remove %i %p",this,index,_Item);
       debugprint(msgbuff);
  #endif

}
//--------------------------------------------------------------------------
void Tclist::AtFree(int _Index) {
  void *item = At(_Index);
  if ( item!=NULL )   {
      delete item;
      items[_Index]=NULL;
      aCount--;
      #ifdef DEBUG
       char msgbuff[1024];
       sprintf(msgbuff,"Tclist %p: Item AtFree %i",this,_Index);
       debugprint(msgbuff);
      #endif
  }
}
//--------------------------------------------------------------------------
void Tclist::AtRemove(int _Index) {
//  items[_Index]=NULL;
//  aCount=0;
  if ( items[_Index]!=NULL)   {
      items[_Index]=NULL;
      aCount--;
      #ifdef DEBUG
           char msgbuff[1024];
           sprintf(msgbuff,"Tclist %p: Item At Remove %i",this,_Index);
           debugprint(msgbuff);
      #endif

  }
}
//--------------------------------------------------------------------------
void Tclist::Swap(int _Index1,int _Index2) {
  void *tmp;
  tmp = At(_Index1);
  items[_Index1] = items[_Index2];
  items[_Index2] = tmp;
}
//--------------------------------------------------------------------------
void Tclist::RemoveAll(void) {
  for  ( int i=0;i<aLimit;i++ ) {
    AtRemove(i);
  }
  aCount = 0;
  #ifdef DEBUG
       char msgbuff[1024];
       sprintf(msgbuff,"Tclist %p: RemoveAll",this);
       debugprint(msgbuff);
  #endif

}
//--------------------------------------------------------------------------
void Tclist::FreeAll(void) {
  for  ( int i=0;i<aLimit;i++ ) {
    AtFree(i);
  }
  aCount=0;
  #ifdef DEBUG
       char msgbuff[1024];
       sprintf(msgbuff,"Tclist %p: FreeAll",this);
       debugprint(msgbuff);
  #endif

}
//--------------------------------------------------------------------------
void Tclist::Assign(Tclist *_oldlist) {
  memset(items,0,sizeof(void *)*aLimit);
  memcpy(items,_oldlist->items,sizeof(void *)*aLimit);
  aCount =_oldlist -> aCount;
}
//--------------------------------------------------------------------------
void *Tclist::NextItem(int _Index) {
  for ( int i=_Index+1;i<aLimit;i++ )
    if ( At(i) ) return At(i);
  return NULL;
}
//--------------------------------------------------------------------------
void *Tclist::PrevItem(int _Index) {
  for ( int i=_Index-1;i>=0;i-- )
    if ( At(i) ) return At(i);
  return NULL;
}
//--------------------------------------------------------------------------
void *Tclist::LastItem(void) {
  for ( int i=aLimit-1;i>=0;i-- )
    if ( At(i) ) return At(i);
  return NULL;
}
//--------------------------------------------------------------------------
void *Tclist::FirstItem(void) {
  for ( int i=0;i<aLimit;i++ )
    if ( At(i) ) return At(i);
  return NULL;
}
//--------------------------------------------------------------------------
int Tclist::Check(void) {
    int counter=0;
    for (int i=0;i<aLimit;i++) {
        if ( items[i] )
            counter++;
    }
    if ( counter != aCount )
        return 0;
    return 1;

}
