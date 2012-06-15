#include "desktop.h"

#include <QFile>
#include <QString>
#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QDir>

#include "baser.h"
#include "player.h"

QDir SAVE_GAME_PARENT("data");
QString autoSaveSlotName()
{
  return SAVE_GAME_PARENT.filePath("autosave");
}

QString gameSlotName(int i_id)
{
  return SAVE_GAME_PARENT.filePath( QString("gameslot") + QString::number(i_id) );
}

QString GameHeader::toString() const
{
  return d_players[0] + ", " + d_players[1] + ", " + d_players[2] +
            " (" + d_timeStamp + ")";
}

bool PrefModel::loadGame(const QString & i_filename)
{
  QFile fl(i_filename);
  if (!fl.open(QIODevice::ReadOnly))
  {
    printf("Load failed\n");
    return false;
  }
  QByteArray ba(fl.readAll());
  fl.close();
  int pos = 0;
  return unserialize(ba, &pos);
}

/* static */
QString PrefModel::getHeader(const QString& i_filename)
{
  QFile fl(i_filename);
  if (!fl.open(QIODevice::ReadOnly))
  {
    qDebug() << i_filename << " doesn't exist";
    return QString();
  }
  QByteArray ba(fl.readAll());
  fl.close();
  int pos = 0;
  GameHeader header;
  if (!unserializeHeader(ba, &pos, header))
  {
    qDebug() << i_filename << " contains invalid header";
    return QString();
  }
  else
  {
    qDebug() << header.toString();
    return header.toString();
  }
}

bool PrefModel::saveGame(const QString & name)
{
  QFile fl(name);
  if (!fl.open(QIODevice::WriteOnly))
  {
    printf("Save failed\n");
    return false;
  }
  QByteArray ba;
  serialize(ba);
  fl.write(ba);
  fl.close();
  return true;
}

void PrefModel::serialize (QByteArray &ba)
{
  serializeInt(ba, 0); // version field
  // header
  serializeString(ba, QDateTime::currentDateTime().toString());
  for (int f = 1; f <= 3; f++)
    serializeString(ba, player(f)->nick());

  // game data
  for (int f = 1; f <= 3; f++)
    player(f)->mScore.serialize(ba);
  serializeInt(ba, nCurrentStart.nValue);
  serializeInt(ba, optMaxPool);
  serializeInt(ba, optStalingrad);
  serializeInt(ba, opt10Whist);
  serializeInt(ba, optWhistGreedy);
}

bool PrefModel::unserializeHeader(QByteArray& ba, int *pos, GameHeader& o_header)
{
  int version = 0;
  if (!unserializeInt(ba, pos, &version))
  {
    qDebug() << "Invalid version: " << version;
    return false;
  }
  if (!unserializeString(ba, pos, o_header.d_timeStamp))
    return false;
  qDebug() << o_header.d_timeStamp;
  for ( int f = 1; f <=3; f++ )
  {
    if (!unserializeString(ba, pos, o_header.d_players[f - 1]))
      return false;
    qDebug() << o_header.d_players[f - 1];
  }
  return true;
}

bool PrefModel::unserialize (QByteArray &ba, int *pos)
{
  GameHeader header;
  if (!unserializeHeader(ba, pos, header))
    return false;
  for (int f = 1; f <= 3; f++)
  {
    Player *plr = player(f);
    if (!plr->mScore.unserialize(ba, pos))
        return false;
  }
  int t;
  if (!unserializeInt(ba, pos, &t))
      return false;
  nCurrentStart.nValue = t;
  if (!unserializeInt(ba, pos, &t))
      return false;
  optMaxPool = static_cast<bool>(t);
  if (!unserializeInt(ba, pos, &t))
      return false;
  optStalingrad = static_cast<bool>(t);
  if (!unserializeInt(ba, pos, &t))
      return false;
  opt10Whist = static_cast<bool>(t);
  if (!unserializeInt(ba, pos, &t))
      return false;
  optWhistGreedy = static_cast<bool>(t);
  return true;
}
