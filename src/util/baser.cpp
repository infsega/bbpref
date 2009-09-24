/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see file COPYING); if not, see 
 *      http://www.gnu.org/licenses 
 */

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


/*
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
*/
