#include <string.h>
#include <stdio.h>

#include "prfconst.h"
#include "card.h"


typedef struct {
 eGameBid game;
 const char *name;
} tGameName;


static tGameName gameNames[] = {
  {g86catch, " "},
  {raspass, "pass-out"},
  {undefined, "-----"},
  {vist, "whist"},
  {gtPass, "pass"},
  {g61, "6 of spades"},
  {g62, "6 of clubs"},
  {g63, "6 of diamonds"},
  {g64, "6 of hearts"},
  {g65, "6 NT"},
  {g71, "7 of spades"},
  {g72, "7 of clubs"},
  {g73, "7 of diamonds"},
  {g74, "7 of hearts"},
  {g75, "7 NT"},
  {g81, "8 of spades"},
  {g82, "8 of clubs"},
  {g83, "8 of diamonds"},
  {g84, "8 of hearts"},
  {g85, "8 NT"},
  {g86, "Misere"},
  {g91, "9 of spades"},
  {g92, "9 of clubs"},
  {g93, "9 of diamonds"},
  {g94, "9 of hearts"},
  {g95, "9 NT"},
  {g101, "10 of spades"},
  {g102, "10 of clubs"},
  {g103, "10 of diamonds"},
  {g104, "10 of hearts"},
  {g105, "10 NT"},
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


const char *sGameName (eGameBid game) {
  for (int i = 0; gameNames[i].name; i++) {
    if (gameNames[i].game == game) return gameNames[i].name;
  }
  return "                 ";
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
