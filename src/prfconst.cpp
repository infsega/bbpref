#include <string.h>
#include <stdio.h>

#include "prfconst.h"
#include "card.h"


//--------------------------------------------------------------------------
typedef struct {
 TGamesType GamesType;
 const char *name;
}  TGameName;
//--------------------------------------------------------------------------
TGameName GameName[] = {
  {g86catch," "},
  {raspass,"PassOut"},
  {undefined,"-----"},
  {vist,"Whist"},
  {gtPass,"Pass"},
  {g61,"6 of spades"},
  {g62,"6 of clubs"},
  {g63,"6 of diamonds"},
  {g64,"6 of hearts"},
  {g65,"6 NT"},
  {g71,"7 of spades"},
  {g72,"7 of clubs"},
  {g73,"7 of diamonds"},
  {g74,"7 of hearts"},
  {g75,"7 NT"},
  {g81,"8 of spades"},
  {g82,"8 of clubs"},
  {g83,"8 of diamonds"},
  {g84,"8 of hearts"},
  {g85,"8 NT"},
  {g86,"Miser"},
  {g91,"9 of spades"},
  {g92,"9 of clubs"},
  {g93,"9 of diamonds"},
  {g94,"9 of hearts"},
  {g95,"9 NT"},
  {g101,"10 of spades"},
  {g102,"10 of clubs"},
  {g103,"10 of diamonds"},
  {g104,"10 of hearts"},
  {g105,"10 NT"}
};
//--------------------------------------------------------------------------
int g61stalingrad;
int g10vist;
int globvist;
int nBuletScore;
TGamesType CurrentGame;

//--------------------------------------------------------------------------
int nGetKoz(void) {
  return CurrentGame - (CurrentGame/10) * 10;
}
//--------------------------------------------------------------------------
int NextGame(TGamesType GamesType) {

  if ( GamesType == g86 )  {
    return g91;
  }
  if (GamesType == g105 )  {
    return GamesType;
  }
  if (GamesType == gtPass )  {
      return g61;
  }
  if ( GamesType != undefined )  {
    int vz = GamesType/10,mast = GamesType - vz*10;
    if ( mast != 5 ) {
      mast++;

    } else {
      mast = SuitSpades;
      vz ++;
    }
    return vz*10+mast;
  } else {
      return g61;
  }
  return gtPass;
}
//--------------------------------------------------------------------------

const char *sGameName(TGamesType GamesType) {
   for (unsigned int i=0;i<=sizeof(GameName)/sizeof(TGameName);i++) {
      if (GameName[i].GamesType == GamesType) return (char *) GameName[i].name;
   }
   return "                 ";
}
//--------------------------------------------------------------------------
int nGetGameCard(TGamesType gType) {
   if ( gType >=g71 && gType <= 75)   return 7;
   if ( gType >=g81 && gType <= 85)   return 8;
   if ( gType ==g86)                  return 0;
   if ( gType >=g91 && gType <= 95)   return 9;
   if ( gType >=g101 && gType <= 105) return 10;
   return 6;
}
//--------------------------------------------------------------------------
int nGetVistCard(TGamesType gType) {
   if ( gType >=g71 && gType <= 75)   return 2;
   if ( gType >=g81 && gType <= 85)   return 1;
   if ( gType ==g86)                  return 0;
   if ( gType >=g91 && gType <= 95)   return 1;
   if ( gType >=g101 && gType <= 105) return 1;
   return 4;
}
//--------------------------------------------------------------------------
int nGetMinCard4Vist(TGamesType gType) {
   if ( gType >=g71 && gType <= 75)   return 2;
   if ( gType >=g81 && gType <= 85)   return 1;

   if ( gType ==g86)                  return 0;
   if ( gType >=g91 && gType <= 95)   return 1;
   if ( gType >=g101 && gType <= 105) return 0;
   return 3;
}
//--------------------------------------------------------------------------
int nGetGamePrice(TGamesType gType) {
   if ( gType >=g71 && gType <= 75)    return 4;
   if ( gType >=g81 && gType <= 85)    return 6;
   if ( gType ==g86)                   return 10;
   if ( gType >=g91 && gType <= 95)    return 8;
   if ( gType >=g101 && gType <= 105)  return 10;
   return 2;
}


//----------------------------------------------------------------------
static bool strcmpQ (const QString &s0, const QString &s1) {
  return s0 == s1;
}


TGamesType GamesTypeByName (const QString &s) {
  if (strcmpQ("raspass",s)) return raspass;
  if (strcmpQ("vist",s)) return vist;
  if (strcmpQ("undefined",s)) return undefined;
  if (strcmpQ("gtPass",s)) return gtPass;
  if (strcmpQ("g61",s)) return g61;
  if (strcmpQ("g62",s)) return g62;
  if (strcmpQ("g63",s)) return g63;
  if (strcmpQ("g64",s)) return g64;
  if (strcmpQ("g65",s)) return g65;
  if (strcmpQ("g71",s)) return g71;
  if (strcmpQ("g72",s)) return g72;
  if (strcmpQ("g73",s)) return g73;
  if (strcmpQ("g74",s)) return g74;
  if (strcmpQ("g75",s)) return g75;
  if (strcmpQ("g81",s)) return g81;
  if (strcmpQ("g82",s)) return g82;
  if (strcmpQ("g83",s)) return g83;
  if (strcmpQ("g84",s)) return g84;
  if (strcmpQ("g85",s)) return g85;
  if (strcmpQ("g86",s)) return g86;
  if (strcmpQ("g91",s)) return g91;
  if (strcmpQ("g92",s)) return g92;
  if (strcmpQ("g93",s)) return g93;
  if (strcmpQ("g94",s)) return g94;
  if (strcmpQ("g95",s)) return g95;
  if (strcmpQ("g101",s)) return g101;
  if (strcmpQ("g102",s)) return g102;
  if (strcmpQ("g103",s)) return g103;
  if (strcmpQ("g104",s)) return g104;
  if (strcmpQ("g105",s)) return g105;
  return undefined;
}
//----------------------------------------------------------------------
int Card2Int(void *vCard) {
        TCard *Card = (TCard*) vCard;
        if (Card)
                return Card -> CName*10 + Card -> CMast;
        return -1;
}
//----------------------------------------------------------------------
