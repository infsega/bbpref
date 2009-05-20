#ifndef NCOUNTER_H
#define NCOUNTER_H

class Tncounter {
  public:
    Tncounter();
    Tncounter(int _nMin,int _nMax );
    Tncounter(int _nValue,int _nMin,int _nMax);
    int nValue,nMin,nMax;
    int operator ++ () {
      nValue++;
      if ( nValue > nMax )  nValue = nMin;
      return nValue;
    };
    int operator -- () {
       nValue--;
       if ( nValue < nMin )  nValue = nMax;
       return nValue;
    };
    friend Tncounter & operator - (Tncounter &,int );
    friend Tncounter & operator + (Tncounter &,int );
};

int NextGamer(Tncounter );
int PrevGamer(Tncounter );

#endif
