//---------------------------------------------------------------------------
#ifndef CardH
#define CardH
//---------------------------------------------------------------------------
#define J 11
#define Q 12
#define K 13
#define A 14

//---------------------------------------------------------------------------
class TCard {
public:
        int CName;      //от 7,8,9,10,11j,12q,13k,14a
        int CMast;      //от 1 до 4
//        char MyVz;      // Должна быть взтка
        void SetCard( int _CName, int _CMast ); // Имя карты. масть
        TCard( int _CName, int _CMast );
        ~TCard();
        friend int operator > (TCard &,TCard &);
        friend int operator < (TCard &,TCard &);

};

//---------------------------------------------------------------------------
#endif
