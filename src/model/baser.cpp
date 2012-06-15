/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
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

#include "baser.h"
#include <QByteArray>
#include <QString>
#include <QDebug>

void serializeInt (QByteArray &ba, int i)
{
  qDebug() << "serialize " << i;
  ba.append( reinterpret_cast<const char*>(&i), sizeof(int) );
}

bool unserializeInt (QByteArray &ba, int *pos, int *i)
{
  if (*pos + sizeof(int) > ba.size())
  {
    qDebug() << "Not enough space for int";
    return false;
  }
  *i = *(reinterpret_cast<const int*>(ba.constData() + *pos));
  qDebug() << "deserialize " << *i;
  *pos += sizeof(int);
  return true;
}

void serializeString(QByteArray& ba, const QString& i_string)
{
  QByteArray bas = i_string.toUtf8();
  serializeInt(ba, bas.size());
  ba.append(bas);
}

bool unserializeString(QByteArray& ba, int *pos, QString& o_string)
{
  int sz = 0;
  if ( !unserializeInt(ba, pos, &sz) )
    return false;
  if (*pos + sz > ba.size())
  {
    qDebug() << "Not enough space for String";
    return false;
  }
  o_string = QString::fromUtf8( ba.constData() + *pos, sz );
  *pos += sz;
  return true;
}
