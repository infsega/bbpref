#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <QApplication>
#include <QString>
#include <QMainWindow>
#include <QMenuBar>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include "qtformtorg.h"
#include <QTimer>
#include <QDialog>
#include "papplication.h"
#include <QObject>
#include "kpref.h"

#include "prfconst.h"
#include "deskview.h"
#include "card.h"

//----------------------------- xpm s  include
/*
#include"pics/101.xpm"
#include"pics/102.xpm"
#include"pics/103.xpm"
#include"pics/104.xpm"
#include"pics/111.xpm"
#include"pics/112.xpm"
#include"pics/113.xpm"
#include"pics/114.xpm"
#include"pics/121.xpm"
#include"pics/122.xpm"
#include"pics/123.xpm"
#include"pics/124.xpm"
#include"pics/131.xpm"
#include"pics/132.xpm"
#include"pics/133.xpm"
#include"pics/134.xpm"
#include"pics/141.xpm"
#include"pics/142.xpm"
#include"pics/143.xpm"
#include"pics/144.xpm"
#include"pics/71.xpm"
#include"pics/72.xpm"
#include"pics/73.xpm"
#include"pics/74.xpm"
#include"pics/81.xpm"
#include"pics/82.xpm"
#include"pics/83.xpm"
#include"pics/84.xpm"
#include"pics/91.xpm"
#include"pics/92.xpm"
#include"pics/93.xpm"
#include"pics/94.xpm"
#include "pics/q101.xpm"
#include "pics/q102.xpm"
#include "pics/q103.xpm"
#include "pics/q104.xpm"
#include "pics/q111.xpm"
#include "pics/q112.xpm"
#include "pics/q113.xpm"
#include "pics/q114.xpm"
#include "pics/q121.xpm"
#include "pics/q122.xpm"
#include "pics/q123.xpm"
#include "pics/q124.xpm"
#include "pics/q131.xpm"
#include "pics/q132.xpm"
#include "pics/q133.xpm"
#include "pics/q134.xpm"
#include "pics/q141.xpm"
#include "pics/q142.xpm"
#include "pics/q143.xpm"
#include "pics/q144.xpm"
#include "pics/q71.xpm"
#include "pics/q72.xpm"
#include "pics/q73.xpm"
#include "pics/q74.xpm"
#include "pics/q81.xpm"
#include "pics/q82.xpm"
#include "pics/q83.xpm"
#include "pics/q84.xpm"
#include "pics/q91.xpm"
#include "pics/q92.xpm"
#include "pics/q93.xpm"
#include "pics/q94.xpm"
#include "pics/1000.xpm"
*/
//----------------------------- end xpm s include
// xpm - xNAME_xpm



static QString GetXpmByName (const char *name) {
  if (!name || !name[0]) return QString(":/pics/x1000.png");
  char nbuf[64];
  if (name[0] == 'q') {
    sprintf(nbuf, ":/pics/%s.png", name);
  } else {
    sprintf(nbuf, ":/pics/x%s.png", name);
  }
  QString fn(name);
  QFile fl(fn);
  if (!fl.exists()) return QString(":/pics/x1000.png");
  return fn;
/*
  if ( !strcmp(name,"1000") ) return x1000_xpm;
  if ( !strcmp(name,"71") ) return x71_xpm;
  if ( !strcmp(name,"72") ) return x72_xpm;
  if ( !strcmp(name,"73") ) return x73_xpm;
  if ( !strcmp(name,"74") ) return x74_xpm;
  if ( !strcmp(name,"81") ) return x81_xpm;
  if ( !strcmp(name,"82") ) return x82_xpm;
  if ( !strcmp(name,"83") ) return x83_xpm;
  if ( !strcmp(name,"84") ) return x84_xpm;
  if ( !strcmp(name,"91") ) return x91_xpm;
  if ( !strcmp(name,"92") ) return x92_xpm;
  if ( !strcmp(name,"93") ) return x93_xpm;
  if ( !strcmp(name,"94") ) return x94_xpm;
  if ( !strcmp(name,"101") ) return x101_xpm;
  if ( !strcmp(name,"102") ) return x102_xpm;
  if ( !strcmp(name,"103") ) return x103_xpm;
  if ( !strcmp(name,"104") ) return x104_xpm;
  if ( !strcmp(name,"111") ) return x111_xpm;
  if ( !strcmp(name,"112") ) return x112_xpm;
  if ( !strcmp(name,"113") ) return x113_xpm;
  if ( !strcmp(name,"114") ) return x114_xpm;
  if ( !strcmp(name,"121") ) return x121_xpm;
  if ( !strcmp(name,"122") ) return x122_xpm;
  if ( !strcmp(name,"123") ) return x123_xpm;
  if ( !strcmp(name,"124") ) return x124_xpm;
  if ( !strcmp(name,"131") ) return x131_xpm;
  if ( !strcmp(name,"132") ) return x132_xpm;
  if ( !strcmp(name,"133") ) return x133_xpm;
  if ( !strcmp(name,"134") ) return x134_xpm;
  if ( !strcmp(name,"141") ) return x141_xpm;
  if ( !strcmp(name,"142") ) return x142_xpm;
  if ( !strcmp(name,"143") ) return x143_xpm;
  if ( !strcmp(name,"144") ) return x144_xpm;
  if ( !strcmp(name,"q71") ) return q71_xpm;
  if ( !strcmp(name,"q72") ) return q72_xpm;
  if ( !strcmp(name,"q73") ) return q73_xpm;
  if ( !strcmp(name,"q74") ) return q74_xpm;
  if ( !strcmp(name,"q81") ) return q81_xpm;
  if ( !strcmp(name,"q82") ) return q82_xpm;
  if ( !strcmp(name,"q83") ) return q83_xpm;
  if ( !strcmp(name,"q84") ) return q84_xpm;
  if ( !strcmp(name,"q91") ) return q91_xpm;
  if ( !strcmp(name,"q92") ) return q92_xpm;
  if ( !strcmp(name,"q93") ) return q93_xpm;
  if ( !strcmp(name,"q94") ) return q94_xpm;
  if ( !strcmp(name,"q101") ) return q101_xpm;
  if ( !strcmp(name,"q102") ) return q102_xpm;
  if ( !strcmp(name,"q103") ) return q103_xpm;
  if ( !strcmp(name,"q104") ) return q104_xpm;
  if ( !strcmp(name,"q111") ) return q111_xpm;
  if ( !strcmp(name,"q112") ) return q112_xpm;
  if ( !strcmp(name,"q113") ) return q113_xpm;
  if ( !strcmp(name,"q114") ) return q114_xpm;
  if ( !strcmp(name,"q121") ) return q121_xpm;
  if ( !strcmp(name,"q122") ) return q122_xpm;
  if ( !strcmp(name,"q123") ) return q123_xpm;
  if ( !strcmp(name,"q124") ) return q124_xpm;
  if ( !strcmp(name,"q131") ) return q131_xpm;
  if ( !strcmp(name,"q132") ) return q132_xpm;
  if ( !strcmp(name,"q133") ) return q133_xpm;
  if ( !strcmp(name,"q134") ) return q134_xpm;
  if ( !strcmp(name,"q141") ) return q141_xpm;
  if ( !strcmp(name,"q142") ) return q142_xpm;
  if ( !strcmp(name,"q143") ) return q143_xpm;
  if ( !strcmp(name,"q144") ) return q144_xpm;
  return x1000_xpm;
*/
}



//---------------------------------------------------------------------
void TDeskView::mySleep(int seconds) {

    time_t tStart=time(NULL);
    Event = 0;
    QTimer *timer = new QTimer();
    //connect( timer, SIGNAL(timeout()),SLOT(PQApplication -> mainWidget () -> slotEndSleep()) );
    timer -> start(PEVENTMILLISECOND);

    while ( ! Event ) {
  //PQApplication -> processOneEvent();
  PQApplication -> processEvents();
        if ( seconds ) {
            if ( difftime (time (NULL),tStart )  >= seconds )
                Event = 1;
        }
    }
    delete timer;


}
//---------------------------------------------------------------------
TDeskView::TDeskView() {
    Event = 0 ;
    CardWidht=CARDWIDTH;
    CardHeight=CARDHEIGHT;
    xBorder = 20;
    yBorder = 20;
      yBorder = 40;
}
//---------------------------------------------------------------------
void TDeskView::ClearBox(int x1,int y1,int x2,int y2) {
  QPainter p(PaintDevice);

  //QBrush brush(QColor(darkGreen));

        QBrush brush(qRgb(0,128,0));

        //QBrash brush;

  QRect NewRect = QRect(x1, y1, x2,y2 );
  p.fillRect(NewRect,brush);
  p.end();
}
//---------------------------------------------------------------------
void TDeskView::ClearScreen(void) {
    ClearBox(0, 0, DesktopWidht,DesktopHeight);
}
//---------------------------------------------------------------------
void TDeskView::SysDrawCardPartialy(TCard *Card,int x,int y,int visible,int border,int x2) {
  char cCardName[16];
  char buff[16];
  cCardName[0]=0;
  if (Card) {
    if ( visible && Card->CName >= 7 && Card->CName <= A && Card->CMast >= 1 && Card->CMast <=4 ) {
      if ( border ) strcat(cCardName,"q");
      itoa(Card->CName,buff,10);
      strcat(cCardName,buff);
      itoa(Card->CMast,buff,10);
      strcat(cCardName,buff);
    } else {
      strcpy(cCardName,"1000");
    }
    try {
      QPixmap Bitmap1 (GetXpmByName(cCardName) );
      QPainter p(PaintDevice);
      if ( x2-x < CARDWIDTH ) {
        p.drawPixmap( QPoint(x,y) ,Bitmap1,QRect(0,0,x2-x,CARDHEIGHT) );
      } else {
        p.drawPixmap( QPoint(x,y) ,Bitmap1);
      }
      p.end();
    }  catch (...)  {
      QPainter p(PaintDevice);
      p.drawText(x,FONTSIZE+y,cCardName);
      p.end();
    }
  } else {
    ClearBox (x,y,x+CARDWIDTH,y+CARDHEIGHT);
  }
}
//---------------------------------------------------------------------
void TDeskView::SysDrawCard(TCard *Card,int x,int y,int visible,int border) {
  char cCardName[16];
  char buff[16];
  cCardName[0]=0;
  if (Card) {
      if ( visible && Card->CName >= 7 && Card->CName <= A && Card->CMast >= 1 && Card->CMast <=4 ) {
      if (Card) {
        if ( border ) strcat(cCardName,"q");
        itoa(Card->CName,buff,10);
        strcat(cCardName,buff);
        itoa(Card->CMast,buff,10);
        strcat(cCardName,buff);
      } else {
        strcpy(cCardName,"1000");
      }
    } else {
      strcpy(cCardName,"1000");
    }
    try {
      QImage Bitmap1 (GetXpmByName(cCardName) );
      QPainter p(PaintDevice);
      p.drawImage( QPoint(x,y) ,Bitmap1);
      p.end();

    }  catch (...)  {
      QPainter p(PaintDevice);
      p.setPen(qRgb(0, 0, 0));
      p.drawText(x,FONTSIZE+y,cCardName);
      p.end();
    }
  } else {
    ClearBox (x,y,x+CARDWIDTH,y+CARDHEIGHT);
  }
}
//---------------------------------------------------------------------
void TDeskView::SysSayMessage(char *sMess,int x,int y) {
    QPainter p(PaintDevice);
    p.setPen(qRgb(255,255,255));
    QBrush b(p.brush());
    b.setColor(qRgb(255,255,255));
    p.setBrush(b);
    p.drawText(x,FONTSIZE+y,sMess);
    p.end();
}
//---------------------------------------------------------------------
void TDeskView::MessageBox(const char *Text,const char *Caption) {
    QMessageBox mb((const char*)Caption,(const char*)Text,QMessageBox::Information,QMessageBox::Ok | QMessageBox::Default,0,0);
    mb.exec();
}
//---------------------------------------------------------------------
void TDeskView::ShowBlankPaper(int nBuletScore) {
    int PaperWidth = 410;
    int PaperHeight = 530;
    xDelta = (DesktopWidht - PaperWidth)/2;
    yDelta = (DesktopHeight - PaperHeight)/2;
      char buff[16];
      QPainter p(PaintDevice);
      QRect NewRect = QRect(xDelta, yDelta, PaperWidth,PaperHeight);
      QBrush brush(qRgb(255, 255, 255));
      p.fillRect(NewRect,brush);
      QBrush b1(brush);
      b1.setColor(qRgb(0xc0,0xc0,0xc0));
      p.setBrush(b1);
      p.setPen(qRgb(0, 0, 0));
      p.drawEllipse(xDelta+188,yDelta+277,35,35);
      p.drawLine(xDelta+410, yDelta+530, xDelta+220, yDelta+307);
      p.drawLine(xDelta+0, yDelta+530, xDelta+190, yDelta+307);
      p.drawLine(xDelta+205, yDelta+277, xDelta+205, yDelta+0);
      p.drawLine(xDelta+40, yDelta+0, xDelta+40, yDelta+483);
      p.drawLine(xDelta+40, yDelta+483, xDelta+370, yDelta+483);
      p.drawLine(xDelta+370, yDelta+483, xDelta+370, yDelta+0);
      p.drawLine(xDelta+370, yDelta+0, xDelta+80, yDelta+0);
      p.drawLine(xDelta+80, yDelta+0, xDelta+80, yDelta+440);
      p.drawLine(xDelta+80, yDelta+440, xDelta+330, yDelta+440);
      p.drawLine(xDelta+330, yDelta+440, xDelta+330, yDelta+0);
      p.drawLine(xDelta+0, yDelta+255, xDelta+40, yDelta+255);
      p.drawLine(xDelta+410, yDelta+255, xDelta+370, yDelta+255);
      p.drawLine(xDelta+205, yDelta+530, xDelta+205, yDelta+483);
      itoa(nBuletScore, buff, 10);
      p.drawText(xDelta+197, FONTSIZE+yDelta+292, buff);
      p.end();
}
//---------------------------------------------------------------------
void TDeskView::StatusBar(const char *Text) {
    kpref -> StatusBar1 -> showMessage (Text);
}
//---------------------------------------------------------------------
void TDeskView::ShowGamerScore(int i,char *sb,char *sm,char *slv,char *srv,char *tv) {
  QPainter p(PaintDevice);
  p.setPen(qRgb(0, 0, 0));
   switch (i) {
         case 1: {
            p.drawText(xDelta+65,FONTSIZE+yDelta+450,sb);
            p.drawText(xDelta+105,FONTSIZE+yDelta+420,sm);
            p.drawText(xDelta+15,FONTSIZE+yDelta+510,slv);
            p.drawText(xDelta+220,FONTSIZE+yDelta+510,srv);

            p.drawText(xDelta+188,FONTSIZE+yDelta+390,tv);

            break;
         }
         case 2: {
            drawRotatedText(&p, xDelta+60, FONTSIZE+yDelta+5, 90,sb );
            drawRotatedText(&p, xDelta+100, FONTSIZE+yDelta+5,90, sm);
            drawRotatedText(&p, xDelta+18, FONTSIZE+yDelta+5, 90,slv);
            drawRotatedText(&p, xDelta+18, FONTSIZE+yDelta+270, 90,srv);

            drawRotatedText(&p, xDelta+170, FONTSIZE+yDelta+120,90, tv);
            break;
         }
         case 3: {
            drawRotatedText(&p, xDelta+355, FONTSIZE+yDelta+440, -90,sb);
            drawRotatedText(&p, xDelta+322, FONTSIZE+yDelta+405, -90,sm);
            drawRotatedText(&p, xDelta+400, FONTSIZE+yDelta+500, -90,slv);
            drawRotatedText(&p, xDelta+400, FONTSIZE+yDelta+240, -90,srv);

            drawRotatedText(&p, xDelta+250, FONTSIZE+yDelta+120, -90,tv);
            break;
         }


      }
  p.end();
}
//---------------------------------------------------------------------
void TDeskView::drawRotatedText(QPainter *p,int x,int y,float angle,char *text) {
  p->translate(x,y);
  p->rotate(angle);
  p->drawText(0,0,text);
  p->rotate(-1*angle);
  p->translate(-1*x,-1*y);
}
//---------------------------------------------------------------------
TGamesType TDeskView::makemove(TGamesType lMove,TGamesType rMove) {
  Q_UNUSED(lMove)
  Q_UNUSED(rMove)
    Formtorg -> exec();
    return Formtorg ->_GamesType;
}
//---------------------------------------------------------------------
