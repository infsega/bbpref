#include <unistd.h>

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


void msSleep (int ms) {
  // 1sec=1000000

  // seconds
  while (ms >= 1000) {
    usleep(1000000/2);
    usleep(1000000/2);
    ms -= 1000;
  }
  // milliseconds
  int sec = (ms*1000); // nseconds
  while (sec > 0) {
    if (sec >= 1000000/2) {
      usleep(1000000/2);
      sec -= 1000000/2;
    } else {
      usleep(sec);
      sec = 0;
    }
  }
}
