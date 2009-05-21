#include <values.h>
#include "ncounter.h"


Tncounter::Tncounter () {
  nMin = nValue = 0;
  nMax = MAXINT;
}


Tncounter::Tncounter (int _nMin, int _nMax) {
  nMin = nValue = _nMin;
  nMax = _nMax;
}


Tncounter::Tncounter (int _nValue, int _nMin, int _nMax) {
  nMin = _nMin;
  nMax = _nMax;
  nValue = (_nValue-nMin)%(nMax-nMin+1)+nMin;
}


Tncounter &operator - (Tncounter &c, int n) {
  while (n-- > 0) --c;
  return c;
}


Tncounter &operator + (Tncounter &c,int n) {
  while (n-- > 0) ++c;
  return c;
}


int succPlayer (const Tncounter &aCurrent) {
  int res = aCurrent.nValue+1;
  if (res > aCurrent.nMax) res = aCurrent.nMin;
  return res;
}

int predPlayer(const Tncounter &aCurrent) {
  int res = aCurrent.nValue-1;
  if (res < aCurrent.nMin) res = aCurrent.nMax;
  return res;
}
