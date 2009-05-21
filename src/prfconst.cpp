#include <string.h>
#include <stdio.h>

#include "prfconst.h"
#include "card.h"


typedef struct {
 eGameBid game;
 const QString name;
} tGameName;


static tGameName gameNames[] = {
  {g86catch, " "},
  {raspass, "pass-out"},
  {undefined, "-----"},
  {vist, "whist"},
  {gtPass, "pass"},
  {g61, "6\1s"},
  {g62, "6\1c"},
  {g63, "6\1d"},
  {g64, "6\1h"},
  {g65, "6NT"},
  {g71, "7\1s"},
  {g72, "7\1c"},
  {g73, "7\1d"},
  {g74, "7\1h"},
  {g75, "7NT"},
  {g81, "8\1s"},
  {g82, "8\1c"},
  {g83, "8\1d"},
  {g84, "8\1h"},
  {g85, "8NT"},
  {g86, "Misere"},
  {g91, "9\1s"},
  {g92, "9\1c"},
  {g93, "9\1d"},
  {g94, "9\1h"},
  {g95, "9NT"},
  {g101, "10\1s"},
  {g102, "10\1c"},
  {g103, "10\1d"},
  {g104, "10\1h"},
  {g105, "10NT"},
  {zerogame, NULL}
};


bool g61stalingrad;
bool g10vist;
bool globvist;
int nBuletScore;
eGameBid CurrentGame;


int trumpSuit (void) {
  return CurrentGame-(CurrentGame/10)*10;
}


int succBid (eGameBid game) {
  switch (game) {
    case g86: return g91; // after misere
    case g105: return g105; // last
    case gtPass: return g61; // first
    default: ;
  }
  if (game != undefined)  {
    int vz = game/10, mast = game-vz*10;
    if (mast != 5) {
      mast++;
    } else {
      mast = SuitSpades;
      vz++;
    }
    return vz*10+mast;
  }
  return g61;
  //return gtPass;
}


const QString &sGameName (eGameBid game) {
  static QString empty;
  for (int i = 0; !gameNames[i].name.isEmpty(); i++) {
    if (gameNames[i].game == game) return gameNames[i].name;
  }
  return empty;
}


int gameTricks (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 7;
  if (gType >= g81 && gType <= 85) return 8;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 9;
  if (gType >= g101 && gType <= 105) return 10;
  return 6;
}


int gameWhists (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 2;
  if (gType >= g81 && gType <= 85) return 1;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 1;
  if (gType >= g101 && gType <= 105) return 1;
  return 4;
}


int gameWhistsMin (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 2;
  if (gType >= g81 && gType <= 85) return 1;
  if (gType == g86) return 0;
  if (gType >= g91 && gType <= 95) return 1;
  if (gType >= g101 && gType <= 105) return 0;
  return 3;
}


int gamePoolPrice (eGameBid gType) {
  if (gType >= g71 && gType <= 75) return 4;
  if (gType >= g81 && gType <= 85) return 6;
  if (gType == g86) return 10;
  if (gType >= g91 && gType <= 95) return 8;
  if (gType >= g101 && gType <= 105) return 10;
  return 2;
}


eGameBid gameName2Type (const QString &s) {
  if (s == "raspass") return raspass;
  if (s == "vist") return vist;
  if (s == "undefined") return undefined;
  if (s == "pass") return gtPass;
  if (s == "g61") return g61;
  if (s == "g62") return g62;
  if (s == "g63") return g63;
  if (s == "g64") return g64;
  if (s == "g65") return g65;
  if (s == "g71") return g71;
  if (s == "g72") return g72;
  if (s == "g73") return g73;
  if (s == "g74") return g74;
  if (s == "g75") return g75;
  if (s == "g81") return g81;
  if (s == "g82") return g82;
  if (s == "g83") return g83;
  if (s == "g84") return g84;
  if (s == "g85") return g85;
  if (s == "g86") return g86;
  if (s == "g91") return g91;
  if (s == "g92") return g92;
  if (s == "g93") return g93;
  if (s == "g94") return g94;
  if (s == "g95") return g95;
  if (s == "g101") return g101;
  if (s == "g102") return g102;
  if (s == "g103") return g103;
  if (s == "g104") return g104;
  if (s == "g105") return g105;
  return undefined;
}
