//deskview.h
// all graphics function depend
#ifndef DESKVIEW_H
#define DESKVIEW_H

#include <QString>
#include <QMainWindow>
#include <QPixmap>
#include <QApplication>
#include <QMessageBox>
#include <QImage>
#include <QPainter>

#include "prfconst.h"
#include "card.h"


class TDeskView {

  public :
  int Event;
  TDeskView();

  QPaintDevice *PaintDevice;

  int nSecondStartWait;
  int DesktopWidht,DesktopHeight;
  int CardWidht,CardHeight;
  int xBorder,yBorder;
  int xLen,yLen;
  int xDelta,yDelta;
  void SysDrawCard(TCard *,int x,int y,int visible,int border);
  void SysDrawCardPartialy(TCard *,int x,int y,int visible,int border,int x2);
  void SysSayMessage(char *,int x,int y);
  void ClearScreen(void);
  void ClearBox(int,int,int,int);
  void mySleep(int seconds);
  void MessageBox(const char *,const char *);
  void ShowBlankPaper(int nBuletScore);
  void ShowGamerScore(int i,char *sb,char *sm,char *slv,char *srv,char *tv);
  //----------------------------- for THuman gamer
  TGamesType makemove(TGamesType lMove,TGamesType rMove);
  void StatusBar(const char *);
  void drawRotatedText(QPainter *p,int x,int y,float angle,char *text);

};


#endif
