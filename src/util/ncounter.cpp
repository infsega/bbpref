#include <values.h>
#include "ncounter.h"

Tncounter::Tncounter() {
   nMin=nValue = 0;
   nMax = MAXINT;
}

Tncounter::Tncounter(int _nMin,int _nMax) {
   nMin = nValue = _nMin;
   nMax = _nMax;
}

Tncounter::Tncounter(int _nValue,int _nMin,int _nMax) {
   nValue = _nValue;
   nMin = _nMin;
   nMax = _nMax;
}

Tncounter & operator - (Tncounter &c,int n) {
  for ( int i=0;i<n;i++ )
    --c;
  return c;
}

Tncounter & operator + (Tncounter &c,int n) {
   for ( int i=0;i<n;i++ )
    ++c;
  return c;
}

int NextGamer(Tncounter aCurrent) {
    Tncounter RetVal = aCurrent;
    ++RetVal;
    return RetVal.nValue;
}

int PrevGamer(Tncounter aCurrent) {
    Tncounter RetVal = aCurrent;
    --RetVal;
    return RetVal.nValue;
}

/*int operator ++ (Tncounter &c) {
   c.nValue++;
   if ( c.nValue > c.nMax )  c.nValue = c.nMin;
   return c.nValue;
}

int operator -- (Tncounter &c) {
   c.nValue--;
   if ( c.nValue < c.nMin )  c.nValue = c.nMax;
   return c.nValue;
} */
