#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <QDebug>

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


static QHash<QString, QImage *> cardI;
static bool cardsLoaded = false;


bool loadCards () {
  //qDebug() << SLOT(slotPushButtonClick95());
  if (cardsLoaded) return true;
  for (int face = 7; face <= 14; face++) {
    for (int suit = 1; suit <= 4; suit++) {
      char fname[64];
      sprintf(fname, ":/pics/%i%i.png", face, suit);
      QString fn(fname);
/*
      QFile fl(fn);
      if (!fl.exists()) {
        qDebug() << "no file:" << fn;
        return false;
      }
*/
      QImage *im = new QImage(fn);
      sprintf(fname, "%i%i", face, suit);
      QString f2(fname);
      cardI[f2] = im;
      sprintf(fname, "q%i%i", face, suit);
      QString f3(fname);
      im = new QImage(fn);
      for (int x = 0; x < im->width(); x++) {
        for (int y = 0; y < im->height(); y++) {
          QRgb p = im->pixel(x, y);
          if (qRed(p) == 255 && qGreen(p) == 255 && qBlue(p) == 255) {
            im->setPixel(x, y, qRgb(255, 255, 0));
          }
        }
      }
      cardI[f3] = im;
    }
  }
  {
  QString fn(":/pics/cards/1000.png");
/*
  QFile fl(fn);
  if (!fl.exists()) {
    return false;
  }
*/
  QImage *im = new QImage(fn);
  cardI["1000"] = im;
  }
  {
  QString fn(":/pics/cards/empty.png");
/*
  QFile fl(fn);
  if (!fl.exists()) return false;
*/
  QImage *im = new QImage(fn);
  cardI["empty"] = im;
  }
  cardsLoaded = true;
  return true;
}


static QImage *GetXpmByNameI (const char *name) {
  if (!cardI.contains(name)) cardI["empty"];
  return cardI[name];
}


///////////////////////////////////////////////////////////////////////////////
TDeskView::TDeskView (int aW, int aH) : mDeskBmp(0) {
  Event = 0;
  CardWidht = CARDWIDTH;
  CardHeight = CARDHEIGHT;
  xBorder = 20;
  yBorder = 20;
  yBorder = 40;
  DesktopWidht = aW;
  DesktopHeight = aH;
  //mDeskBmp = new QPixmap(DesktopWidht, DesktopHeight);
  if (!cardsLoaded) {
    if (!loadCards()) abort();
  }
  ClearScreen();
}


TDeskView::~TDeskView () {
  if (mDeskBmp) delete mDeskBmp;
}


void TDeskView::emitRepaint () {
  emit deskChanged();
}


void TDeskView::mySleep (int seconds) {
  time_t tStart = time(NULL);
  Event = 0;
  QTimer *timer = 0;
  if (seconds > 0) {
    timer = new QTimer();
    //connect(timer, SIGNAL(timeout()), SLOT(PQApplication->mainWidget()->slotEndSleep()));
    timer->start(seconds*1000);
  }
  while (!Event) {
    emitRepaint();
    qApp->processEvents();
    qApp->sendPostedEvents();
    qApp->flush();
    if (seconds > 0) {
      if (difftime(time(NULL), tStart) >= seconds) Event = 1;
    }
    usleep(10000); //1000000
  }
  if (timer) delete timer;
  emitRepaint();
}


void TDeskView::ClearScreen () {
  if (!mDeskBmp || (mDeskBmp->width() != DesktopWidht || mDeskBmp->height() != DesktopHeight)) {
    if (mDeskBmp) delete mDeskBmp;
    mDeskBmp = new QPixmap(DesktopWidht, DesktopHeight);
  }
  ClearBox(0, 0, DesktopWidht, DesktopHeight);
}


void TDeskView::ClearBox (int x1, int y1, int x2, int y2) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  QBrush brush(qRgb(0, 128, 0));
  QRect NewRect = QRect(x1, y1, x2, y2);
  p.fillRect(NewRect, brush);
  p.end();
}


void TDeskView::drawCard (TCard *card, int x, int y, bool opened, bool hilight) {
  char cCardName[16];

  if (!mDeskBmp) return;

  cCardName[0] = 0;
  if (!card) {
    ClearBox(x, y, x+CARDWIDTH, y+CARDHEIGHT);
    return;
  }

  strcpy(cCardName, opened?"empty":"1000");
  if (opened && card->CName >= 7 && card->CName <= FACE_ACE && card->CMast >= 1 && card->CMast <= 4) {
    sprintf(cCardName, "%s%i%i", hilight?"q":"", card->CName, card->CMast);
  }
  QImage *bmp = GetXpmByNameI(cCardName);
  if (!bmp) {
    ClearBox(x, y, x+CARDWIDTH, y+CARDHEIGHT);
    return;
  }
  QPainter p(mDeskBmp);
  p.drawImage(x, y, *bmp);
  p.end();
}


void TDeskView::drawText (const QString &str, int x, int y, QRgb textColor, QRgb outlineColor) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  p.setPen(outlineColor);
  QBrush b(p.brush());
  b.setColor(outlineColor);
  p.setBrush(b);
  y += FONTSIZE;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx || dy) p.drawText(x+dx, y+dy, str);
    }
  }
  p.setPen(textColor);
  b.setColor(textColor);
  p.setBrush(b);
  p.drawText(x, y, str);
  p.end();
}


void TDeskView::MessageBox (const QString &text, const QString &caption) {
  QMessageBox mb(caption, text, QMessageBox::Information, QMessageBox::Ok | QMessageBox::Default, 0, 0);
  mb.exec();
}


void TDeskView::ShowBlankPaper (int nBuletScore) {
  int PaperWidth = 410;
  int PaperHeight = 530;
  if (!mDeskBmp) return;
  xDelta = (DesktopWidht-PaperWidth)/2;
  yDelta = (DesktopHeight-PaperHeight)/2;
  //char buff[16];
  QPainter p(mDeskBmp);
  QRect NewRect = QRect(xDelta, yDelta, PaperWidth, PaperHeight);
  QBrush brush(qRgb(255, 255, 255));
  p.fillRect(NewRect, brush);
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
  //itoa(nBuletScore, buff, 10);
  //p.drawText(xDelta+197, FONTSIZE+yDelta+292, buff);
  p.end();
  drawText(QString::number(nBuletScore), xDelta+197, yDelta+292);
}


void TDeskView::StatusBar (const QString &text) {
  kpref->StatusBar1->showMessage(text);
}


void TDeskView::showPlayerScore (int i, const QString &sb, const QString &sm, const QString &slv, const QString &srv, const QString &tv) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  p.setPen(qRgb(0, 0, 0));
  switch (i) {
    case 1:
      p.drawText(xDelta+65, FONTSIZE+yDelta+450, sb);
      p.drawText(xDelta+105, FONTSIZE+yDelta+420, sm);
      p.drawText(xDelta+15, FONTSIZE+yDelta+510, slv);
      p.drawText(xDelta+220, FONTSIZE+yDelta+510, srv);
      p.drawText(xDelta+188, FONTSIZE+yDelta+390, tv);
      break;
    case 2:
      drawRotatedText(p, xDelta+60, FONTSIZE+yDelta+5, 90, sb );
      drawRotatedText(p, xDelta+100, FONTSIZE+yDelta+5, 90, sm);
      drawRotatedText(p, xDelta+18, FONTSIZE+yDelta+5, 90, slv);
      drawRotatedText(p, xDelta+18, FONTSIZE+yDelta+270, 90, srv);
      drawRotatedText(p, xDelta+170, FONTSIZE+yDelta+120, 90, tv);
      break;
    case 3:
      drawRotatedText(p, xDelta+355, FONTSIZE+yDelta+440, -90, sb);
      drawRotatedText(p, xDelta+322, FONTSIZE+yDelta+405, -90, sm);
      drawRotatedText(p, xDelta+400, FONTSIZE+yDelta+500, -90, slv);
      drawRotatedText(p, xDelta+400, FONTSIZE+yDelta+240, -90, srv);
      drawRotatedText(p, xDelta+250, FONTSIZE+yDelta+120, -90, tv);
      break;
    default: ;
  }
  p.end();
}


void TDeskView::drawRotatedText (QPainter &p, int x, int y, float angle, const QString &text) {
  p.translate(x, y);
  p.rotate(angle);
  p.drawText(0, 0, text);
  p.rotate(-1*angle);
  p.translate(-1*x, -1*y);
}


TGamesType TDeskView::makemove (TGamesType lMove, TGamesType rMove) {
  Q_UNUSED(lMove)
  Q_UNUSED(rMove)
  if (!Formtorg->exec()) qApp->quit();
  return Formtorg->_GamesType;
}
