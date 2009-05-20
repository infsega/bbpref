//---------------------------------------------------------------------------
#ifndef CardListH
#define CardListH
//---------------------------------------------------------------------------
#include "prfconst.h"
#include "ctlist.h"
#include "card.h"
//---------------------------------------------------------------------------
typedef struct {
  int m;
  int l;
  int v;
} TMParam;
//масть длинна взятки

class  TCardList : public Tclist {
    public:
        TCardList(int _Limit);
        ~TCardList ();
//        ~TCardList();
        void   mySort(void);
        TCard * Exist( int _CName, int _CMast );
        TCard * Exist( int PackedName );
        TCard * MinCard( int _CMast );
        TCard * MaxCard( int _CMast );
        TCard * MinCard( void );
        TCard * MaxCard( void );
        TCard * MoreThan( int _CName, int _CMast ); //первая больше чем переданная
        TCard * LessThan( int _CName, int _CMast ); //первая меньше чем переданная
        TCard * MoreThan( TCard *Card ); //первая больше чем переданная
        TCard * LessThan( TCard *Card ); //первая меньше чем переданная
        TCard * FirstCard(void);
        TCard * LastCard(void);
        TCard * NextCard(void *);
        TCard * NextCard(int _nIndex);
        void AssignMast(Tclist *_oldlist,TMast Mast); // Assign only selected mast from oldlist


        int AllCard( int _CMast );
        int AllCard(void);
        int EmptyMast(int _CMast);                        //возврат масти (за исключение данной) в которой нет карт

};
//---------------------------------------------------------------------------
#endif
