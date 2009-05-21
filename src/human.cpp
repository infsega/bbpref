#include <stdlib.h>
#include <stdio.h>

#include "prfconst.h"

#include "formbid.h"
#include "kpref.h"


#include "deskview.h"
#include "player.h"
#include "card.h"
#include "cardlist.h"
#include "ncounter.h"
#include "plscore.h"
#include "human.h"


HumanPlayer::HumanPlayer(int _nGamer) : Player(_nGamer) {
}


HumanPlayer::HumanPlayer(int _nGamer,TDeskView *_DeskView) : Player(_nGamer, _DeskView) {
}


tGameBid HumanPlayer::makemove (tGameBid lMove, tGameBid rMove) {
  //ход при торговле
  tGameBid tmpGamesType;
  WaitForMouse = 1;
  //PQApplication->mainWidget ()->setMouseTracking ( TRUE );

  formBid->EnableAll();
  if (qMax(lMove, rMove) != gtPass) formBid->DisableLessThan(qMax(lMove, rMove));
  if (GamesType != undefined) formBid->DisalbeGames(g86);
  formBid->DisalbeGames(vist);
  formBid->EnableGames(gtPass);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(FALSE);

  do {
    tmpGamesType = DeskView->makemove(lMove, rMove);
    if (tmpGamesType == 0) {
      // вернуть снос
      GetBackSnos();
/*
      Repaint(DeskView, (DeskView->DesktopWidht-DeskView->xLen)/2,
        DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight, DeskView->xLen, DeskView->yLen);
      makemove((TCard *)NULL, (TCard *)NULL, (Player *)NULL, (Player *)NULL);
      Repaint(DeskView, (DeskView->DesktopWidht-DeskView->xLen)/2,
        DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight, DeskView->xLen, DeskView->yLen);
      makemove((TCard *)NULL, (TCard *)NULL, (Player *)NULL, (Player *)NULL);
      Repaint(DeskView, (DeskView->DesktopWidht-DeskView->xLen)/2,
        DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight, DeskView->xLen, DeskView->yLen);
*/
      RepaintSimple(true);
    } else if (tmpGamesType == 1) {
      // показать пулю
      kpref->DeskTop->nflShowPaper = 1;
      kpref->slotShowBollet();
    }
  } while (tmpGamesType <= 1);
  GamesType = tmpGamesType;
  //PQApplication->mainWidget()->setMouseTracking(FALSE);
  WaitForMouse = 0;
  formBid->EnableAll();
  return GamesType;
}


TCard *HumanPlayer::makemove (TCard *lMove, TCard *rMove, Player *aLeftGamer, Player *aRightGamer) { //ход
  Q_UNUSED(aLeftGamer)
  Q_UNUSED(aRightGamer)

  TCard *RetVal = 0;
/*
  int nDx=0, j, ii;
  int Left = 0, Top = 0, Width;
*/

  WaitForMouse = 1;
  //PQApplication->mainWidget ()->setMouseTracking ( TRUE );
  //Height = DeskView->yLen;
  while (!RetVal) {
/*
    //PQApplication->processOneEvent();
    PQApplication->processEvents();
        // View Prot
          DeskView->xLen=DeskView->DesktopWidht/2-2*DeskView->xBorder;
          DeskView->yLen=DeskView->DesktopHeight/2-2*DeskView->yBorder;
          Width  = DeskView->xLen;
          switch (nGamer) {
            case 1: {
              Left   = (DeskView->DesktopWidht-DeskView->xLen)/2;
              Top    = DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight;
              break;
            }
            case 2: {
              Left   = DeskView->xBorder;
              Top    = DeskView->yBorder;

              break;
            }
            case 3: {
              Left   = DeskView->DesktopWidht/2+DeskView->xBorder;
              Top    = DeskView->yBorder;
              break;
            }
          }
        // end View Port
*/
/*        DeskView->xLen=DeskView->DesktopWidht/2-2*DeskView->xBorder;
        DeskView->yLen=DeskView->DesktopHeight/2-2*DeskView->yBorder;
        Left   = (DeskView->DesktopWidht-DeskView->xLen)/2;
        Top    = DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight;
        Width  = DeskView->xLen;*/
/*
        if (aCards->aCount)
            nDx = (DeskView->xLen - DeskView->xBorder*2) / (aCards->aCount+1);
*/
    int cNo = cardAt(X, Y);
/*
        j=0;
        ii=-1;
        for (int i = 0;i<aCards->aCount ;i++) {
            int x1= Left+j*nDx,x2,y1 = Top+DeskView->yBorder,y2;
            x2 = x1 + CARDWIDTH;
            y2 = y1 + CARDHEIGHT;
            if ( x1 < X && X < x2 && y1 <Y && Y <y2)
                ii = i;   // Remember only last
            j++;
        }
*/
    if ( cNo != -1 ) {
      TCard *Validator;
      int koz = nGetKoz();
      Validator = RetVal = (TCard *)aCards->At(cNo);
      if (lMove || rMove) {
        Validator = lMove;
        if (lMove == NULL) Validator = rMove;
      }
      // пропускаем ход через правила
      if ((Validator->CMast == RetVal->CMast) ||
          (!aCards->MinCard(Validator->CMast) && (RetVal->CMast == koz || ((!aCards->MinCard(koz)))))) {
      } else {
        RetVal = NULL;
      }
    }
  }
  if (RetVal) {
    aCards->Remove(RetVal);
    aCardsOut->Insert(RetVal);
  }
  X = Y = 0;
  WaitForMouse = 0;
  return RetVal;
}


tGameBid HumanPlayer::makeout4miser(void) { // после сноса чего играем
  WaitForMouse = 1;
  makemove( (TCard *)NULL,(TCard *)NULL,(Player *)NULL,(Player *)NULL);
  //!Repaint(DeskView,(DeskView->DesktopWidht-DeskView->xLen)/2,DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight,DeskView->xLen,DeskView->yLen);
  RepaintSimple(true);
  DeskView->mySleep(1);
  makemove( (TCard *)NULL,(TCard *)NULL,(Player *)NULL,(Player *)NULL);
  //!Repaint(DeskView,(DeskView->DesktopWidht-DeskView->xLen)/2,DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight,DeskView->xLen,DeskView->yLen);
  RepaintSimple(true);
  WaitForMouse = 0;
  return g86;
}
//-------------------------------------------------------------------------
tGameBid HumanPlayer::makeout4game(void) { // после сноса чего играем
  WaitForMouse = 1;
  tGameBid tmpGamesType;
//    PQApplication->mainWidget ()->setMouseTracking ( TRUE );
  X=Y=0;
  HumanPlayer::makemove( (TCard *)NULL,(TCard *)NULL,(Player *)NULL,(Player *)NULL);
  //Repaint(DeskView,(DeskView->DesktopWidht-DeskView->xLen)/2,DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight,DeskView->xLen,DeskView->yLen);
  RepaintSimple(true);
  DeskView->mySleep(1);
//  Repaint(DeskView,);
  //Repaint (TDeskView *DeskView,int Left,int Top,int Width,int Height) {
  HumanPlayer::makemove( (TCard *)NULL,(TCard *)NULL,(Player *)NULL,(Player *)NULL);
  //Repaint(DeskView,(DeskView->DesktopWidht-DeskView->xLen)/2,DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight,DeskView->xLen,DeskView->yLen);
  RepaintSimple(true);
  formBid->EnableAll();
  formBid->DisalbeGames(vist);
  formBid->DisalbeGames(gtPass);
  if (GamesType != g86) {
    formBid->DisalbeGames(g86);
  }
  formBid->DisableLessThan(  GamesType );
//  GamesType = DeskView->makemove((tGameBid)NULL,(tGameBid)NULL);
  formBid->showbullet->setEnabled(TRUE);
  formBid->bgetback->setEnabled(TRUE);


  do {
    tmpGamesType = DeskView->makemove(zerogame, zerogame);
    if (tmpGamesType == 0 ) { // вернуть снос
        GetBackSnos();
        //Repaint(DeskView,(DeskView->DesktopWidht-DeskView->xLen)/2,DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight,DeskView->xLen,DeskView->yLen);
        RepaintSimple(true);
        makemove( (TCard *)NULL,(TCard *)NULL,(Player *)NULL,(Player *)NULL);
        //Repaint(DeskView,(DeskView->DesktopWidht-DeskView->xLen)/2,DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight,DeskView->xLen,DeskView->yLen);
        RepaintSimple(true);
        makemove( (TCard *)NULL,(TCard *)NULL,(Player *)NULL,(Player *)NULL);
        //Repaint(DeskView,(DeskView->DesktopWidht-DeskView->xLen)/2,DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight,DeskView->xLen,DeskView->yLen);
        RepaintSimple(true);

    } else if (tmpGamesType == 1) { // показать пулю
      kpref->slotShowBollet();

    }
  } while(  tmpGamesType <= 1 );
  GamesType = tmpGamesType;

//  GamesType = DeskView->makemove(zerogame, zerogame );

  formBid->EnableAll();
//    PQApplication->mainWidget ()->setMouseTracking ( FALSE );

  WaitForMouse = 0;
  return GamesType;
}
//-------------------------------------------------------------------------
tGameBid HumanPlayer::makemove (tGameBid MaxGame,int HaveAVist,int nGamerVist) {
  Q_UNUSED(HaveAVist)
  Q_UNUSED(nGamerVist)
//      formBid->DisalbeLessThan()
      if ( MaxGame == g86 ) {
        GamesType = g86catch;
      } else {
        formBid->DisalbeAll();
          formBid->EnableGames(gtPass);
          formBid->EnableGames(vist);
          GamesType = DeskView->makemove(zerogame,zerogame);
          formBid->EnableAll();
        }
      return GamesType;
}
//-------------------------------------------------------------------------
void HumanPlayer::HintCard (int lx,int ly) {
  if (!WaitForMouse) {
    if (oldii == -1) return;
    oldii = -1;
    //Repaint(DeskView, int Left, int Top, int Width, int Height, bool opened=false, int selNo=-1);
    RepaintSimple(true);
    return;
  }
  int cNo = cardAt(lx, ly);
  if (cNo == oldii) return;
  oldii = cNo;
  RepaintSimple(true);
/*
  TCard *RetVal = NULL;
  int nDx=0,j,ii;
  int Left=0,Top=0,Width;
  if ( WaitForMouse ) {
  // View Prot
    DeskView->xLen=DeskView->DesktopWidht/2-2*DeskView->xBorder;
    DeskView->yLen=DeskView->DesktopHeight/2-2*DeskView->yBorder;
    Width  = DeskView->xLen;
    switch (nGamer) {
      case 1: {
        Left   = (DeskView->DesktopWidht-DeskView->xLen)/2;
        Top    = DeskView->DesktopHeight-(DeskView->yBorder*2)-DeskView->CardHeight;
        break;
      }
      case 2: {
        Left   = DeskView->xBorder;
        Top    = DeskView->yBorder;
        break;
      }
      case 3: {
        Left   = DeskView->DesktopWidht/2+DeskView->xBorder;
        Top    = DeskView->yBorder;
        break;
      }
    }
  // end View Port
    if (aCards->aCount)
      nDx = (DeskView->xLen - DeskView->xBorder*2) / (aCards->aCount+1);
    j=0;
    ii=-1;
    for (int i = 0;i<aCards->aCount ;i++) {
      int x1= Left+j*nDx,x2,y1 = Top+DeskView->yBorder,y2;
      x2 = x1 + CARDWIDTH;
      y2 = y1 + CARDHEIGHT;
      if ( x1 < lx && lx < x2 && y1 <ly && ly <y2) {            // ÷ряюьшэрхь ┤юы╪ъю яю└ыхфэ╬╬
        ii = i;
      }
      j++;
    } // end for
    if ( ii == -1 || oldii == ii) {
      return;
    } else {
//            RetVal = (TCard *)aCards->At(ii);
      RetVal = NULL;
    }
    j=0;
    for (int i = 0;i<aCards->aCount ;i++) {
//           TCard *my;
//      if ( i >= qMin(oldii,ii) ) {
        RetVal = (TCard *)aCards->At(i);
        if ( RetVal ) {
          int aBorder=0;
          if ( i == ii ) {
            aBorder = 1;
            oldii = ii;
          }
          DeskView->drawCard(RetVal,Left+j*nDx,Top+DeskView->yBorder,1,aBorder);
        }
//      }//if ( i >= qMin(oldii,ii) )
    j++;
    }
  } // waiting for mouse
  */
}
//-------------------------------------------------------------------------
