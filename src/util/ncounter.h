#ifndef NCOUNTER_H
#define NCOUNTER_H


class Tncounter {
public:
  Tncounter ();
  Tncounter (int _nMin, int _nMax );
  Tncounter (int _nValue, int _nMin, int _nMax);

  int operator ++ () {
    if (++nValue > nMax) nValue = nMin;
    return nValue;
  }

  int operator -- () {
     if (--nValue < nMin) nValue = nMax;
     return nValue;
  }

  friend Tncounter &operator - (Tncounter &c, int n);
  friend Tncounter &operator + (Tncounter &c, int n);

public:
  int nValue;
  int nMin;
  int nMax;
};


int succPlayer (const Tncounter &aCurrent);
int predPlayer (const Tncounter &aCurrent);


#endif
