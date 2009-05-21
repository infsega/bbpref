
#include "prfconst.h"
#include "plscore.h"

//-------------------------------------------------------------------
TPlScore::TPlScore() {
   Bullet = new Tclist(MAXGAMELEN);
   Mountan= new Tclist(MAXGAMELEN);
   LeftVists= new Tclist(MAXGAMELEN);
   RightVists= new Tclist(MAXGAMELEN);
   Vists = 0;
}
//-------------------------------------------------------------------
TPlScore::~TPlScore() {
   delete Bullet ;
   delete Mountan;
   delete LeftVists;
   delete RightVists;
}
//-------------------------------------------------------------------
int TPlScore::AddBullet(int RetValAddRec) {
  int nScore = nGetBull();
  if ( (nScore + RetValAddRec) <=  nBuletScore ) {
    Bullet ->  Insert(new int(nScore + RetValAddRec) );
    return 0;
  } else {
    if (nBuletScore != nGetBull() )
      Bullet ->  Insert(new int(nBuletScore));
    return RetValAddRec - (nBuletScore-nScore);
  }
}
//-------------------------------------------------------------------
void TPlScore::MountanDown(int RetValAddRec) {
  int nScore = nGetMount()- RetValAddRec;
  if ( nScore >= 0) {
    Mountan ->  Insert(new int(nScore) );
  } else {
    nScore = (RetValAddRec - nGetMount())*10/2;
    if ( nGetMount() ) Mountan ->  Insert(new int(0));
    LeftVists -> Insert(new int (nGetLeftVists()+nScore));
    RightVists -> Insert(new int (nGetRightVists()+nScore));
  }
}
//-------------------------------------------------------------------
void TPlScore::MountanUp(int RetValAddRec) {
  int nScore = nGetMount();
  Mountan ->  Insert(new int(nScore+RetValAddRec) );
}
//-------------------------------------------------------------------
void TPlScore::AddVist(int index,int myNumber,int Skolko) {
  Tclist *NaKogo;
  int *pnCurrent;
  int nScore=0;
  NaKogo  = RightVists;
  if ( (myNumber==1 && index == 2) ||(myNumber==2 && index == 3)|| (myNumber==3 && index == 1) )
    NaKogo  = LeftVists;
  pnCurrent = (int *) NaKogo -> LastItem();
  nScore = (pnCurrent==NULL ? 0: *pnCurrent) + Skolko*10;
  NaKogo -> Insert(new int (nScore));
}
//-------------------------------------------------------------------
int TPlScore::AddRecords( eGameBid aGamerType,eGameBid aMyType, int nGamerVz/*��������������*/,int nMyVz/*��c��� ����*/, int nGamer,int myNumber,int nqVist/*���-�� ���������*/) {
  int nGamePrice = gamePoolPrice(aGamerType);  // ���� ����
  int nGameCard=gameTricks(aGamerType); // ������ ������ ���� ���  ������ ����
  int nVistCard=gameWhists(aGamerType); // ����������� ��� �� ������ ��������� �� 2 ��
  int nScore=0,*pnCurrent=NULL;
  Tclist *NaKogo;
  if ( aMyType >=g61 && aMyType != g86 ) {
    // ������ � ���- �� ����� !
    if ( nGamerVz >= nGameCard ) {    // � ������ !
      return AddBullet(nGamePrice);

   } else {  // � ������� ����
      pnCurrent = (int *) Mountan -> LastItem();
      nScore = nGamePrice*(nGameCard-nMyVz) + (pnCurrent==NULL ? 0: *pnCurrent);
      if (nScore) Mountan -> Insert(new int (nScore));
    }
    return 0;
  }
    //-------------------------------------------------------------------------
  if ( aMyType == g86 ) {
    if (nMyVz) {
      MountanUp(nGamePrice*nMyVz);
      return 0;
    } else {
      return AddBullet(10);
    }

  }
    //-------------------------------------------------------------------------
  if ( aMyType == raspass )  {
      pnCurrent = (int *) Mountan -> LastItem();
      nScore = nGamePrice*(nMyVz) + (pnCurrent==NULL ? 0: *pnCurrent);
      if (nMyVz && nScore) {
          Mountan -> Insert(new int (nScore));
          return 0;
        }
      if ( !nMyVz )
          return AddBullet(1);
  }

  NaKogo  = RightVists;
  if ( (myNumber==1 && nGamer == 2) ||(myNumber==2 && nGamer == 3)|| (myNumber==3 && nGamer == 1) )
    NaKogo  = LeftVists;
  pnCurrent = (int *) NaKogo -> LastItem();

    //-------------------------------------------------------------------------
  if ( aMyType == gtPass && aGamerType != g86 )  {
    if ( !globvist && nGameCard-nGamerVz > 0)  {   // �������������  - ��������
        nScore = nGamePrice*((nGameCard-nGamerVz)+nMyVz) + (pnCurrent==NULL ? 0: *pnCurrent);
    }
    if (nScore)  NaKogo -> Insert(new int (nScore));
  }
    //-------------------------------------------------------------------------
  if ( aMyType == vist && aGamerType != g86 )  {
    if ( nGameCard-nGamerVz > 0)  {   // �������������  - ��������
        if (nqVist == 2) { // 2 ���������
            //������ ������ �� ���� ������ + �������
            nScore = nGamePrice*((nGameCard-nGamerVz)+nMyVz) + (pnCurrent==NULL ? 0: *pnCurrent);
        } else { // 1 ���������
                nScore = nGamePrice*((nGameCard-nGamerVz)+nMyVz) + (pnCurrent==NULL ? 0: *pnCurrent);
                //������ �� ���� + �������
        }
    } else { // �������������  - ���� ���e� ��� ������
      if (nqVist == 2) {
          nScore = nGamePrice*(nMyVz) + (pnCurrent==NULL ? 0: *pnCurrent);
      } else {
          nScore = nGamePrice*(10-nGamerVz) + (pnCurrent==NULL ? 0: *pnCurrent);
      }
        // ��������� �� ������� �������� �� ������� ���������� !!!!
       if ( nVistCard > (10-nGamerVz) ) {
        if (nqVist == 2) {
              MountanUp(
                  (int)(((float)(nVistCard/2-nMyVz)) * nGamePrice)
              );
        } else { //1 ���������
          if ( (nVistCard - nMyVz ) > 0)
              MountanUp(nGamePrice*(nVistCard - (10-nGamerVz) ));
        }
      }
    }
     if (nScore) NaKogo -> Insert(new int (nScore));
  }
  return 0;
}
//-------------------------------------------------------------
int TPlScore::nGetBull() {
     int *pnScore = (int *) Bullet -> LastItem();
     if ( pnScore ) return *pnScore;
     return 0;
}
//-------------------------------------------------------------
int TPlScore::nGetMount() {
     int *pnScore = (int *) Mountan -> LastItem();
     if ( pnScore ) return *pnScore;
     return 0;
}
//-------------------------------------------------------------
int TPlScore::nGetLeftVists() {
     int *pnScore = (int *) LeftVists -> LastItem();
     if ( pnScore ) return *pnScore;
     return 0;
}
//-------------------------------------------------------------
int TPlScore::nGetRightVists() {
     int *pnScore = (int *) RightVists -> LastItem();
     if ( pnScore ) return *pnScore;
     return 0;;
}
