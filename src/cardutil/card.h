//---------------------------------------------------------------------------
#ifndef CARD_H
#define CARD_H
//---------------------------------------------------------------------------
#define FACE_JACK   11
#define FACE_QUEEN  12
#define FACE_KING   13
#define FACE_ACE    14

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
