#include "baser.h"


void serializeInt (QByteArray &ba, int i) {
  for (int f = 3; f >= 0; f--) {
    unsigned char c = (i&0xff); i >>= 8;
    ba.append((char)c);
  }
}


bool unserializeInt (QByteArray &ba, int *pos, int *i) {
  unsigned char buf[4];
  if (*pos+4 > ba.size()) return false;
  const unsigned char *u = (const unsigned char *)ba.constData();
  u += *pos; *pos += 4;
  for (int f = 3; f >= 0; f--) buf[f] = *u++;
  *i = 0;
  for (int f = 0; f < 4; f++) {
    *i <<= 8;
    *i |= buf[f];
  }
  return true;
}
