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
#include <QObject>
#include <QHash>

#include "papplication.h"
#include "kpref.h"

#include "prfconst.h"
#include "deskview.h"
#include "card.h"


static QHash<QString, QImage *> cardCacheI;
static QHash<QString, QPixmap *> cardCacheP;

static QImage *GetXpmByNameI (const char *name) {
  char nbuf[64];
  if (!name || !name[0]) strcpy(nbuf, ":/pics/x1000.png");
  if (name[0] == 'q') {
    sprintf(nbuf, ":/pics/%s.png", name);
  } else {
    sprintf(nbuf, ":/pics/x%s.png", name);
  }
  QString fn(name);
  if (cardCacheI.contains(fn)) return cardCacheI[fn];
  QFile fl(fn);
  if (!fl.exists()) return GetXpmByNameI("1000");
  QImage *bmp = new QImage(fn);
  cardCacheI[fn] = bmp;
  return bmp;
}


static QPixmap *GetXpmByNameP (const char *name) {
  char nbuf[64];
  if (!name || !name[0]) strcpy(nbuf, ":/pics/x1000.png");
  if (name[0] == 'q') {
    sprintf(nbuf, ":/pics/%s.png", name);
  } else {
    sprintf(nbuf, ":/pics/x%s.png", name);
  }
  QString fn(name);
  if (cardCacheP.contains(fn)) return cardCacheP[fn];
  QFile fl(fn);
  if (!fl.exists()) return GetXpmByNameP("1000");
  QPixmap *bmp = new QPixmap(fn);
  cardCacheP[fn] = bmp;
  return bmp;
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
      QPixmap Bitmap1(*GetXpmByNameP(cCardName));
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
      QImage Bitmap1(*GetXpmByNameI(cCardName));
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
