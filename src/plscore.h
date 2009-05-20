#ifndef PLSCORE_H
#define PLSCORE_H


#include "ctlist.h"
#include "prfconst.h"



class TPlScore {
 public:
   TPlScore();
   ~TPlScore();
   int AddRecords( TGamesType aGamerType,TGamesType aMyType, int nGamerVz,int nMyVz, int nGamer,int myNumber,int nqVist);
   Tclist *Bullet;
   Tclist *Mountan;
   Tclist *LeftVists;
   Tclist *RightVists;
   int nGetBull();
   int nGetMount();
   int nGetLeftVists();
   int nGetRightVists();
   int AddBullet(int RetValAddRec);
   void AddVist(int index,int myNumber,int Skolko);
   void MountanDown(int RetValAddRec);
   void MountanUp(int RetValAddRec);
   int Vists;
};


#endif
