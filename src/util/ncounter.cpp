#include <values.h>
#include "ncounter.h"


WrapCounter::WrapCounter () {
  nMin = nValue = 0;
  nMax = MAXINT;
}


WrapCounter::WrapCounter (int _nMin, int _nMax) {
  nMin = nValue = _nMin;
  nMax = _nMax;
}


WrapCounter::WrapCounter (int _nValue, int _nMin, int _nMax) {
  nMin = _nMin;
  nMax = _nMax;
  nValue = (_nValue-nMin)%(nMax-nMin+1)+nMin;
}


WrapCounter &operator - (WrapCounter &c, int n) {
  while (n-- > 0) --c;
  return c;
}


WrapCounter &operator + (WrapCounter &c,int n) {
  while (n-- > 0) ++c;
  return c;
}


int succPlayer (const WrapCounter &aCurrent) {
  int res = aCurrent.nValue+1;
  if (res > aCurrent.nMax) res = aCurrent.nMin;
  return res;
}

int predPlayer(const WrapCounter &aCurrent) {
  int res = aCurrent.nValue-1;
  if (res < aCurrent.nMin) res = aCurrent.nMax;
  return res;
}
