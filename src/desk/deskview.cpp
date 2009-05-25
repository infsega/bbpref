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
#include <QTimer>
#include <QDialog>
#include <QObject>
#include <QHash>

#include "deskview.h"

#include "baser.h"
#include "card.h"
#include "formbid.h"
#include "kpref.h"
#include "prfconst.h"


static QHash<QString, QImage *> cardI;
static QImage *bidIcons[106]; // wasted!
static bool cardsLoaded = false;


// change white to yellow
static void yellowize (QImage *im, QRgb newColor=qRgb(255, 255, 0)) {
  for (int y = im->height()-1; y >= 0; y--) {
    for (int x = im->width()-1; x >= 0; x--) {
      QRgb p = im->pixel(x, y);
      if (qRed(p) == 255 && qGreen(p) == 255 && qBlue(p) == 255) {
        im->setPixel(x, y, newColor);
      }
    }
  }
}


static bool loadCards () {
  //qDebug() << SLOT(slotPushButtonClick95());
  if (cardsLoaded) return true;
  for (int f = 0; f < 106; f++) bidIcons[f] = 0;
  // load cards from resources
  for (int face = 7; face <= 14; face++) {
    for (int suit = 1; suit <= 4; suit++) {
      QString fname, resname;
      resname.sprintf("%i%i", face, suit);
      fname.sprintf(":/pics/cards/%i%i.png", face, suit);
      cardI[resname] = new QImage(fname);
      // build highlighted image
      QImage *im = new QImage(fname);
      yellowize(im);
      resname.sprintf("q%i%i", face, suit);
      cardI[resname] = im;
    }
  }
  // load cardback
  cardI["1000"] = new QImage(QString(":/pics/cards/1000.png"));
  cardI["empty"] = new QImage(QString(":/pics/cards/empty.png"));
  // load bid icons
  for (int f = 6; f <= 10; f++) {
    for (int s = 1; s <= 5; s++) {
      QString fname;
      fname.sprintf(":/pics/bids/s%i%i.png", f, s);
      bidIcons[f*10+s] = new QImage(fname);
    }
  }
  // done
  cardsLoaded = true;
  return true;
}


static QImage *GetXpmByNameI (const char *name) {
  if (!cardI.contains(name)) cardI["empty"];
  return cardI[name];
}


///////////////////////////////////////////////////////////////////////////////
DeskView::DeskView (int aW, int aH) : mDeskBmp(0) {
  mDigitsBmp = new QImage(QString(":/pics/digits/digits.png"));
  mBidBmp = new QImage(QString(":/pics/bidinfo.png"));
  mKeyBmp[0] = new QImage(QString(":/pics/presskey.png"));
  mKeyBmp[1] = new QImage(QString(":/pics/presskey.png"));
  yellowize(mKeyBmp[1], qRgb(127, 127, 127));
  mIMoveBmp = new QImage(QString(":/pics/imove.png"));

  if (!cardsLoaded) {
    if (!loadCards()) abort();
  }

  Event = 0;
  CardWidht = CARDWIDTH;
  CardHeight = CARDHEIGHT;
  xBorder = 20;
  yBorder = 20;
  DesktopWidth = aW;
  DesktopHeight = aH;
  //mDeskBmp = new QPixmap(DesktopWidth, DesktopHeight);
  ClearScreen();
}


DeskView::~DeskView () {
  if (mDeskBmp) delete mDeskBmp;
  delete mIMoveBmp;
  delete mKeyBmp[0];
  delete mKeyBmp[1];
  delete mBidBmp;
  delete mDigitsBmp;
}


void DeskView::emitRepaint () {
  emit deskChanged();
}


void DeskView::drawIMove (int x, int y) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  p.drawImage(x, y, *mIMoveBmp);
  p.end();
}


void DeskView::drawPKeyBmp (bool show) {
  if (!mDeskBmp) return;
  time_t tt = time(0);
  int phase = tt%2;
  QImage *i = mKeyBmp[phase];
  if (show) {
    QPainter p(mDeskBmp);
    p.drawImage(4, DesktopHeight-(i->height()+8), *i);
    p.end();
  } else {
    ClearBox(4, DesktopHeight-(i->height()+8), i->width(), i->height());
  }
}


void DeskView::drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy) {
  QRect tgt(x0, y0, 8, 14);
  QRect src(cx, cy, 8, 14);
  p.drawImage(tgt, *mDigitsBmp, src);
}


void DeskView::drawNumber (int x0, int y0, int n, bool red) {
  if (!mDeskBmp || n < 0 || n > 1024) return;
  char buf[12], *pd;
  sprintf(buf, "%i", n);
  pd = buf;
  QPainter p(mDeskBmp);
  while (*pd) {
    int d = (*pd++)-'0';
    int px = d*8, py = red?14:0;
    if (red) drawBmpChar(p, x0+1, y0+1, px, 0);
    drawBmpChar(p, x0, y0, px, py);
    x0 += 8;
  }
  p.end();
}


static int numWidth (int n) {
  if (n < 0 || n > 1024) return 0;
  char buf[16];
  sprintf(buf, "%i", n);
  return strlen(buf)*8;
}


void DeskView::drawGameBid (eGameBid game) {
  if (!mDeskBmp) return;
  if (game != raspass && (game < 61 || game > 105)) return; // unknown game
  QPainter p(mDeskBmp);
  int x, y;
  QImage *i;
  switch (game) {
    case raspass:
      drawBmpChar(p, bidBmpX+44-4, bidBmpY+34-7, 80, 0);
      break;
    case g86: // misere
      drawBmpChar(p, bidBmpX+44-4, bidBmpY+34-7, 80, 14);
      break;
    default:
      i = bidIcons[(int)game];
      if (i) {
        x = 44-(i->width()/2)+3;
        y = 34-(i->height()/2)-1;
        p.drawImage(bidBmpX+x, bidBmpY+y, *i);
      }
      break;
  }
  p.end();
}


/* game:
 *  <0: none
 *  =0: misere
 * suit:
 *  =0: nt
 * plrAct: 0-3
 */
void DeskView::drawBidsBmp (int plrAct, int p0t, int p1t, int p2t, eGameBid game) {
  if (!mDeskBmp) return;
  QImage *i = mBidBmp;
  bidBmpX = DesktopWidth-(i->width()+8);
  bidBmpY = DesktopHeight-(i->height()+8);
  QPainter p(mDeskBmp);
  p.drawImage(bidBmpX, bidBmpY, *i);
  p.end();
/*
 * p0t: 10,6 (left top)
 * p1t: 77,6 (right top)
 * p2t: 44,50 (bottom center)
 * bid: 44,34 (center)
*/
  // human
  if (p0t >= 0) {
    int wdt = numWidth(p0t);
    int x = 44-(wdt/2);
    drawNumber(bidBmpX+x, bidBmpY+50, p0t, plrAct==1);
  }
  // left-top ai
  if (p1t >= 0) {
    drawNumber(bidBmpX+10, bidBmpY+6, p1t, plrAct==2);
  }
  // right-top ai
  if (p2t >= 0) {
    int wdt = numWidth(p2t);
    int x = 77-wdt;
    drawNumber(bidBmpX+x, bidBmpY+6, p2t, plrAct==3);
  }
  drawGameBid(game);
}


void DeskView::timerSlot () {
}


void DeskView::mySleep (int seconds) {
  time_t tStart = time(0);
  Event = 0;
  QTimer *timer = 0;
  if (seconds > 0) {
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()));
    timer->start(seconds*1000);
  }
  drawPKeyBmp(seconds == -1);
  emitRepaint();
  while (!Event) {
    qApp->processEvents(QEventLoop::WaitForMoreEvents);
    qApp->sendPostedEvents();
    //qApp->flush();
    //qApp->processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents, 900);
    if (seconds > 0) {
      if (difftime(time(0), tStart) >= seconds) Event = 3;
    }
    //usleep(10000); //1000000
    msSleep(40);
    if (seconds == 0) break;
    if (seconds < 0) {
      if (seconds == -1) {
        drawPKeyBmp(true);
        emitRepaint();
      }
      if (seconds < -1 && Event == 2) Event = 0;
    }
  }
  if (timer) delete timer;
  drawPKeyBmp(false);
  emitRepaint();
}


void DeskView::aniSleep (int milliseconds) {
  emitRepaint();
  do {
    qApp->processEvents(QEventLoop::WaitForMoreEvents);
    qApp->sendPostedEvents();
    qApp->flush();
    //qApp->processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents, 900);
    if (milliseconds > 0) {
      if (milliseconds > 100) {
        msSleep(100);
        milliseconds -= 100;
      } else {
        msSleep(milliseconds);
        milliseconds = 0;
      }
    }
  } while (milliseconds > 0);
  Event = 0;
  //emitRepaint();
}


void DeskView::ClearScreen () {
  if (!mDeskBmp || (mDeskBmp->width() != DesktopWidth || mDeskBmp->height() != DesktopHeight)) {
    if (mDeskBmp) delete mDeskBmp;
    mDeskBmp = new QPixmap(DesktopWidth, DesktopHeight);
  }
  ClearBox(0, 0, DesktopWidth, DesktopHeight);
}


void DeskView::ClearBox (int x1, int y1, int x2, int y2) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  QBrush brush(qRgb(0, 128, 0));
  QRect NewRect = QRect(x1, y1, x2, y2);
  p.fillRect(NewRect, brush);
  p.end();
}


void DeskView::drawCard (Card *card, int x, int y, bool opened, bool hilight) {
  char cCardName[16];

  if (!mDeskBmp) return;

  cCardName[0] = 0;
  if (!card) {
    ClearBox(x, y, x+CARDWIDTH, y+CARDHEIGHT);
    return;
  }

  strcpy(cCardName, opened?"empty":"1000");
  //opened = true;
  if (opened && card->face() >= 7 && card->face() <= FACE_ACE && card->suit() >= 1 && card->suit() <= 4) {
    sprintf(cCardName, "%s%i%i", hilight?"q":"", card->face(), card->suit());
    //if (hilight) fprintf(stderr, "SELECTED! [%s]\n", cCardName);
  }
  QImage *bmp = GetXpmByNameI(cCardName);
  if (!bmp) {
    //fprintf(stderr, "not found: [%s]\n", cCardName);
    bmp = GetXpmByNameI("empty");
    if (!bmp) {
      ClearBox(x, y, x+CARDWIDTH, y+CARDHEIGHT);
      return;
    }
  }
  //fprintf(stderr, "found: [%s]\n", cCardName);
  QPainter p(mDeskBmp);
  p.drawImage(x, y, *bmp);
  p.end();
}


void DeskView::drawText (const QString &str, int x, int y, QRgb textColor, QRgb outlineColor) {
  if (!mDeskBmp) return;

  QString s(str);
  s.replace("\1s", QChar((ushort)0x2660));
  s.replace("\1c", QChar((ushort)0x2663));
  s.replace("\1d", QChar((ushort)0x2666));
  s.replace("\1h", QChar((ushort)0x2665));
  s = s.trimmed();

  QPainter p(mDeskBmp);
  p.setPen(outlineColor);
  QBrush b(p.brush());
  b.setColor(outlineColor);
  p.setBrush(b);
  y += FONTSIZE;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx || dy) p.drawText(x+dx, y+dy, s);
    }
  }
  p.setPen(textColor);
  b.setColor(textColor);
  p.setBrush(b);
  p.drawText(x, y, s);
  p.end();
}


void DeskView::MessageBox (const QString &text, const QString &caption) {
  QMessageBox mb(caption, text, QMessageBox::Information, QMessageBox::Ok | QMessageBox::Default, 0, 0);
  mb.exec();
}


void DeskView::ShowBlankPaper (int optMaxPool) {
  int PaperWidth = 410;
  int PaperHeight = 530;
  if (!mDeskBmp) return;
  xDelta = (DesktopWidth-PaperWidth)/2;
  yDelta = (DesktopHeight-PaperHeight)/2;
  //char buff[16];
  QPainter p(mDeskBmp);
  QRect NewRect = QRect(xDelta, yDelta, PaperWidth, PaperHeight);
  QBrush brush(qRgb(255, 255, 255));
  p.fillRect(NewRect, brush);
  QBrush b1(brush);
  b1.setColor(qRgb(255, 255, 0));
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
  //itoa(optMaxPool, buff, 10);
  //p.drawText(xDelta+197, FONTSIZE+yDelta+292, buff);
  //drawText(QString::number(optMaxPool), xDelta+197, yDelta+290);
  p.drawText(xDelta+197, yDelta+300, QString::number(optMaxPool));
  p.end();
}


void DeskView::StatusBar (const QString &text) {
  qDebug() << text;
  //kpref->StatusBar1->showMessage(text);
}


void DeskView::showPlayerScore (int i, const QString &sb, const QString &sm, const QString &slv, const QString &srv, const QString &tv) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  p.setPen(qRgb(0, 0, 0));
  QFont fnt(p.font());
  fnt.setBold(false);
  p.setFont(fnt);
  switch (i) {
    case 1:
      p.setPen(qRgb(0, 128, 0));
      p.drawText(xDelta+65, FONTSIZE+yDelta+450, sb);
      p.setPen(qRgb(255, 0, 0));
      p.drawText(xDelta+105, FONTSIZE+yDelta+420, sm);
      p.setPen(qRgb(0, 0, 0));
      p.drawText(xDelta+15, FONTSIZE+yDelta+510, slv);
      p.drawText(xDelta+220, FONTSIZE+yDelta+510, srv);
      p.setPen(qRgb(0, 0, 255));
      fnt.setBold(true);
      p.setFont(fnt);
      p.drawText(xDelta+188, FONTSIZE+yDelta+390, tv);
      break;
    case 2:
      p.setPen(qRgb(0, 128, 0));
      drawRotatedText(p, xDelta+60, FONTSIZE+yDelta+5, 90, sb);
      p.setPen(qRgb(255, 0, 0));
      drawRotatedText(p, xDelta+100, FONTSIZE+yDelta+5, 90, sm);
      p.setPen(qRgb(0, 0, 0));
      drawRotatedText(p, xDelta+18, FONTSIZE+yDelta+5, 90, slv);
      drawRotatedText(p, xDelta+18, FONTSIZE+yDelta+270, 90, srv);
      p.setPen(qRgb(0, 0, 255));
      fnt.setBold(true);
      p.setFont(fnt);
      drawRotatedText(p, xDelta+170, FONTSIZE+yDelta+120, 90, tv);
      break;
    case 3:
      p.setPen(qRgb(0, 128, 0));
      drawRotatedText(p, xDelta+355, FONTSIZE+yDelta+440, -90, sb);
      p.setPen(qRgb(255, 0, 0));
      drawRotatedText(p, xDelta+322, FONTSIZE+yDelta+405, -90, sm);
      p.setPen(qRgb(0, 0, 0));
      drawRotatedText(p, xDelta+400, FONTSIZE+yDelta+500, -90, slv);
      drawRotatedText(p, xDelta+400, FONTSIZE+yDelta+240, -90, srv);
      p.setPen(qRgb(0, 0, 255));
      fnt.setBold(true);
      p.setFont(fnt);
      drawRotatedText(p, xDelta+250, FONTSIZE+yDelta+120, -90, tv);
      break;
    default: ;
  }
  fnt.setBold(false);
  p.setFont(fnt);
  p.end();
}


void DeskView::drawRotatedText (QPainter &p, int x, int y, float angle, const QString &text) {
  p.translate(x, y);
  p.rotate(angle);
  p.drawText(0, 0, text);
  p.rotate(-1*angle);
  p.translate(-1*x, -1*y);
}


eGameBid DeskView::selectBid (eGameBid lMove, eGameBid rMove) {
  Q_UNUSED(lMove)
  Q_UNUSED(rMove)
  //if (!formBid->exec()) qApp->quit();
  formBid->exec();
  return formBid->_GamesType;
}


void DeskView::drawMessageWindow (int x0, int y0, const QString &msg, bool dim) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  // change suits to unicode chars
  QString s(msg);
  s.replace("\1s", QChar((ushort)0x2660));
  s.replace("\1c", QChar((ushort)0x2663));
  s.replace("\1d", QChar((ushort)0x2666));
  s.replace("\1h", QChar((ushort)0x2665));
  s = s.trimmed();
  // now i want to know text extents
  QRect drawR(0, 0, 0, 0);
  QRect boundR;
  p.drawText(drawR, Qt::AlignLeft | Qt::AlignTop | Qt::TextExpandTabs, s, &boundR);
  // window size
  // .|.....text.....|.
  int w = boundR.width()+4+4+2+2, h = boundR.height()+2+2+2+2;
  if (x0 < 0) {
    if (x0 == -666) x0 = (DesktopWidth-w)/2; else x0 += DesktopWidth-w;
  }
  if (y0 < 0) {
    if (y0 == -666) y0 = (DesktopHeight-h)/2; else y0 += DesktopHeight-h;
  }
  // draw shadow
  QBrush brush(qRgba(0, 0, 0, 128));
  QRect winR(x0, y0, w, h);
  winR.adjust(-4, 8, -4, 8);
  p.fillRect(winR, brush);
  winR.adjust(4, -8, 4, -8);
  // draw window
  brush.setColor(qRgb(dim?164:255, dim?164:255, dim?164:255));
  p.fillRect(winR, brush);
  // draw frame
  p.setPen(qRgb(0, 0, 0));
  p.drawLine(x0+1, y0+1, x0+w-2, y0+1);
  p.drawLine(x0+1, y0+h-2, x0+w-2, y0+h-2);
  p.drawLine(x0+1, y0+1, x0+1, y0+h-2);
  p.drawLine(x0+w-2, y0+1, x0+w-2, y0+h-2);
  // draw text
  boundR.adjust(x0+2+4, y0+2+2, x0+2+4, y0+2+2);
  p.drawText(boundR, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextExpandTabs, s);
  // done
  p.end();
}
