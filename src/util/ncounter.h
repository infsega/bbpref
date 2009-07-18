#ifndef NCOUNTER_H
#define NCOUNTER_H


class WrapCounter {
public:
  WrapCounter ();
  WrapCounter (int _nMin, int _nMax );
  WrapCounter (int _nValue, int _nMin, int _nMax);

  int operator ++ () {
    if (++nValue > nMax) nValue = nMin;
    return nValue;
  }

  int operator -- () {
     if (--nValue < nMin) nValue = nMax;
     return nValue;
  }

  friend WrapCounter &operator - (WrapCounter &c, int n);
  friend WrapCounter &operator + (WrapCounter &c, int n);

public:
  int nValue;
  int nMin;
  int nMax;
};


int succPlayer (const WrapCounter &aCurrent);
int predPlayer (const WrapCounter &aCurrent);


#endif
