#ifndef BASER_H
#define BASER_H

#include <QByteArray>


void serializeInt (QByteArray &ba, int i);
bool unserializeInt (QByteArray &ba, int *pos, int *i);

//void msSleep (int ms);


#endif
