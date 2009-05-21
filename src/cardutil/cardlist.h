//---------------------------------------------------------------------------
#ifndef CARDLIST_H
#define CARDLIST_H

#include "prfconst.h"
#include "ctlist.h"
#include "card.h"


typedef struct {
  int m;
  int l;
  int v;
} TMParam;
//масть длинна взятки

class  CardList : public Tclist {
    public:
        CardList(int _Limit);
        ~CardList ();
//        ~CardList();
        void   mySort(void);
        Card * Exist( int _CName, int _CMast );
        Card * Exist( int PackedName );
        Card * MinCard( int _CMast );
        Card * MaxCard( int _CMast );
        Card * MinCard( void );
        Card * MaxCard( void );
        Card * MoreThan( int _CName, int _CMast ); //первая больше чем переданная
        Card * LessThan( int _CName, int _CMast ); //первая меньше чем переданная
        Card * MoreThan( Card *card ); //первая больше чем переданная
        Card * LessThan( Card *card ); //первая меньше чем переданная
        Card * FirstCard(void);
        Card * LastCard(void);
        Card * NextCard(void *);
        Card * NextCard(int _nIndex);
        void AssignMast(Tclist *_oldlist,eSuit Mast); // Assign only selected mast from oldlist


        int AllCard( int _CMast );
        int AllCard(void);
        int EmptyMast(int _CMast);                        //возврат масти (за исключение данной) в которой нет карт

};


#endif
