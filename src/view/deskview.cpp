/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 3 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program (see file COPYING); if not, see 
 *      http://www.gnu.org/licenses 
 */

#include <QDebug>

#include <QApplication>
#include <QMessageBox>
//#include <QPixmap>
#include <QPainter>
#include <QPalette>
#include <QTimer>
#include <QVBoxLayout>

#include "deskview.h"
#include "desktop.h"

#include "baser.h"
#include "formbid.h"
//#include "kpref.h"
#include "prfconst.h"
#include "player.h"
//class SleepEventLoop;

#include "scorewidget.h"


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

namespace {
  class SleepEventFilter : public QObject {
  public:
    SleepEventFilter (SleepEventLoop *eloop, QObject *parent=0) : QObject(parent) { mLoop = eloop; }

  protected:
    bool eventFilter (QObject *obj, QEvent *e);

  private:
    SleepEventLoop *mLoop;
    };

  bool SleepEventFilter::eventFilter (QObject *obj, QEvent *e) {
    Q_UNUSED(obj)
    //
    if (e->type() == QEvent::KeyPress) {
      QKeyEvent *event = static_cast<QKeyEvent *>(e);
      mLoop->doEventKey(event);
    } else if (e->type() == QEvent::MouseButtonPress) {
      QMouseEvent *event = static_cast<QMouseEvent *>(e);
      mLoop->doEventMouse(event);
    }
    return false; // event is not tasty
  }
} // end of namespace

///////////////////////////////////////////////////////////////////////////////
void SleepEventLoop::doEventKey (QKeyEvent *event) {
  //qDebug() << "key" << event->key() << mIgnoreKey;
  if (mIgnoreKey) return;
  switch (event->key()) {
    case Qt::Key_Escape:
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Space:
      mKeyPressed = true;
      quit();
      break;
    default: ;
  }
}


void SleepEventLoop::doEventMouse (QMouseEvent *event) {
  if (mIgnoreMouse) return;
  if (event->button() == Qt::LeftButton) {
    mMousePressed = true;
    mMouseX = event->x();
    mMouseY = event->y();
    //qDebug() << "mouse! x:" << mMouseX << "y:" << mMouseY;
    quit();
  }
}


void SleepEventLoop::keyPicUpdate () {
  if (optPrefClub == true) {
    mDeskView->drawPKeyBmp(true);
    mDeskView->update();
  }
}

void SleepEventLoop::quit () {
  mIgnoreKey = false;
  mIgnoreMouse = false;
  QEventLoop::quit();
}


class DeskViewPrivate
{
public:
  DeskViewPrivate(DeskView *parent) : m_eloop(0), m_efilter(0), m_timer(0), m_scorew(0)
  {
    m_eloop = new SleepEventLoop(parent);
    m_efilter = new SleepEventFilter(m_eloop);
    m_timer = new QTimer(m_eloop);
    m_scorew = new ScoreWidget(parent->m_model, parent);
  }

  ~DeskViewPrivate()
  {
    m_efilter->deleteLater();
    m_eloop->deleteLater();
    m_timer->deleteLater();
  }

  SleepEventLoop *m_eloop;
  SleepEventFilter *m_efilter;
  QTimer *m_timer;
  ScoreWidget *m_scorew;
  //QDialog *m_score;
};

bool DeskView::loadCards () {
  qDebug() << "load";
  //qDebug() << SLOT(slotPushButtonClick95());
  // load cards from resources
  QString deckPath;
  if (optPrefClub) {
  	deckPath = "prefclub/";
	CARDWIDTH = 56;
	CARDHEIGHT = 67;
  }
  else {
  	deckPath = "classic/";
	CARDWIDTH = 71;
	CARDHEIGHT = 96;
  }
  for (int face = 7; face <= 14; face++) {
    for (int suit = 1; suit <= 4; suit++) {
      QString fname, resname;
      resname.sprintf("%i%i", face, suit);
      fname.sprintf(QString(":/pics/cards/"+deckPath+"%i%i.png").toAscii(), face, suit);
      cardI[resname] = new QPixmap(fname);
      // build highlighted image
      QImage im(fname);
      yellowize(&im);
      resname.sprintf("q%i%i", face, suit);
      //cardI[resname] = new QPixmap;
      cardI[resname] = new QPixmap(QPixmap::fromImage(im));
    }
  }
  // load cardback
  cardI["1000"] = new QPixmap(QString(":/pics/cards/"+deckPath+"1000.png"));
  cardI["empty"] = new QPixmap(QString(":/pics/cards/"+deckPath+"empty.png"));
  // load bid icons
  for (int f = 6; f <= 10; f++) {
    for (int s = 1; s <= 5; s++) {
      QString fname;
      fname.sprintf(":/pics/bids/s%i%i.png", f, s);
      bidIcons[f*10+s] = new QPixmap(fname);
    }
  }
  cardI.squeeze();
  bidIcons.squeeze();
  //qDebug() << "cardI.size() = " << cardI.size();
  //qDebug() << "cardI.capacity() = " << cardI.capacity();  
  //qDebug() << "bidIcons.size() = " << bidIcons.size();
  //qDebug() << "bidIcons.capacity() = " << bidIcons.capacity();
  
  // done
  return true;
}


void DeskView::freeCards () {
  qDebug() << "free";
  foreach (QPixmap *i, bidIcons)
    delete i;
  bidIcons.clear();

  foreach (QPixmap *i, cardI)
    delete i;
  cardI.clear();
}


QPixmap *DeskView::GetImgByName (const char *name) {
  if (!cardI.contains(name)) cardI["empty"];
  return cardI[name];
}



///////////////////////////////////////////////////////////////////////////////
//DeskView::DeskView (int aW, int aH) : mDeskBmp(0), d_ptr(new DeskViewPrivate(this))
DeskView::DeskView (QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f), d_ptr(new DeskViewPrivate(this)),
                                                           m_model(0), mDeskBmp(0)
{
  mDigitsBmp = new QPixmap(QString(":/pics/digits/digits.png"));
  mBidBmp = new QPixmap(QString(":/pics/bidinfo.png"));

  mKeyBmp[0] = new QPixmap(QString(":/pics/presskey.png"));
  
  QImage key1(QString(":/pics/presskey.png"));  
  yellowize(&key1, qRgb(127, 127, 127));
  mKeyBmp[1] = new QPixmap(QPixmap::fromImage(key1));
  
  mIMoveBmp = new QPixmap(QString(":/pics/imove.png"));

  //if (!loadCards()) abort();
  const bool result = loadCards();
  Q_ASSERT(result);

  //Event = 0;
  CardWidht = CARDWIDTH;
  CardHeight = CARDHEIGHT;
  m_leftRightMargin = 20;
  m_topBottomMargin = 20;
 /* QPalette palette;
  palette.setBrush(QPalette::Window, QPixmap(":/pics/cloth.png"));
  setPalette(palette);*/
  //DesktopWidth = aW;
  //DesktopHeight = aH;
  //qDebug() << width() << height();
  mDeskBmp = new QPixmap(width(), height());
  ClearScreen();
  setMouseTracking(true);

  //connect(this, SIGNAL(deskChanged()), this, SLOT(update()));
}


DeskView::~DeskView () {
  qDebug() << "NO!!!";
  delete mDeskBmp;
  freeCards();
  delete mIMoveBmp;
  delete mKeyBmp[0];
  delete mKeyBmp[1];
  delete mBidBmp;
  delete mDigitsBmp;
  delete d_ptr;
}


void DeskView::setModel(PrefModel *desktop)
{
  m_model = desktop;
  delete d_ptr;
  d_ptr = new DeskViewPrivate(this);
}


void DeskView::drawIMove (/*int x, int y*/) {
  int x, y;
  if (!mDeskBmp) return;
  switch (m_model->nCurrentStart.nValue) {
      case 1:
        x = width()/2-15;
        y = height() - m_topBottomMargin - CARDHEIGHT - 30; //- 40;
        break;
      case 2:
        x = m_leftRightMargin+20;
        y = m_topBottomMargin + CARDHEIGHT + 20; //+40;
        break;
      case 3:
        x = width() - m_leftRightMargin - 20;
        y = m_topBottomMargin + CARDHEIGHT + 20; //+40;
        break;
      default:
        qDebug() << "Invalid nCurrentStart.nValue =" << m_model->nCurrentStart.nValue;
        x = y = -1;
        break;
  }
  QPainter p(mDeskBmp);
  p.drawPixmap(x, y, *mIMoveBmp);
  p.end();
}


void DeskView::drawPKeyBmp (bool show) {
  static int phase = 0;
  if (!mDeskBmp) return;
/*
  time_t tt = time(0);
  int phase = tt%2;
*/
  QPixmap *i = mKeyBmp[phase];
  if (show) {
    QPainter p(mDeskBmp);	
    p.drawPixmap(4, height()-(i->height()+8), *i);
    p.end();
    phase = 1-phase;
  } else {
    ClearBox(4, height()-(i->height()+8), i->width(), i->height());
  }
}


void DeskView::drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy) {
  QRect tgt(x0, y0, 8, 14);
  QRect src(cx, cy, 8, 14);
  p.drawPixmap(tgt, *mDigitsBmp, src);
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
  QPixmap *i;
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
        //p.drawImage(bidBmpX+x, bidBmpY+y, *i);
        p.drawPixmap(bidBmpX+x, bidBmpY+y, *i);
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
  QPixmap *i = mBidBmp;
  bidBmpX = width()-(i->width()+8);
  bidBmpY = height()-(i->height()+8);
  QPainter p(mDeskBmp);
  p.drawPixmap(bidBmpX, bidBmpY, *i);
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


void DeskView::mySleep (int seconds) {
  //Event = 0;
  installEventFilter(d_ptr->m_efilter);

  if (seconds == 0)
  {
    /*if (optPrefClub == false)
	  seconds=1;
    else {*/
    drawPKeyBmp(false);
    update();
    qApp->processEvents(QEventLoop::WaitForMoreEvents);
    qApp->sendPostedEvents();
    removeEventFilter(d_ptr->m_efilter);
    return;
    //}
  }
  if (seconds > 0) {
    connect(d_ptr->m_timer, SIGNAL(timeout()), d_ptr->m_eloop, SLOT(quit()));
    d_ptr->m_timer->start(seconds*1000);
  } else if (seconds == -1) {
    connect(d_ptr->m_timer, SIGNAL(timeout()), d_ptr->m_eloop, SLOT(keyPicUpdate()));
    d_ptr->m_timer->start(1000);
  } else if (seconds < -1) {
    d_ptr->m_eloop->mIgnoreKey = true;
    //d_ptr->m_timer->start(0);
  }
  if (optPrefClub == true)
  drawPKeyBmp(seconds == -1);

  update();
  if (d_ptr->m_eloop->isRunning())
    d_ptr->m_eloop->exit();
  d_ptr->m_eloop->exec();
  removeEventFilter(d_ptr->m_efilter);
  disconnect(d_ptr->m_timer, SIGNAL(timeout()), 0, 0);

  if (seconds == -1) {
    drawPKeyBmp(false);
    update();
  }
  //d_ptr->m_eloop->exit();
}


void DeskView::aniSleep (int milliseconds) {
  d_ptr->m_eloop->mIgnoreMouse = true;
  d_ptr->m_eloop->mIgnoreKey = true;
  installEventFilter(d_ptr->m_efilter);
  connect(d_ptr->m_timer, SIGNAL(timeout()), d_ptr->m_eloop, SLOT(quit()));
  d_ptr->m_timer->start(milliseconds);

  update();
  d_ptr->m_eloop->exec();
  removeEventFilter(d_ptr->m_efilter);
  disconnect(d_ptr->m_timer, SIGNAL(timeout()), 0, 0);
}


void DeskView::ClearScreen () {
  if (!mDeskBmp || (mDeskBmp->width() != width() || mDeskBmp->height() != height())) {
    delete mDeskBmp;
    mDeskBmp = new QPixmap(width(), height());
  }
  ClearBox(0, 0, width(), height());
}


void DeskView::ClearBox (int x1, int y1, int x2, int y2) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  //QBrush brush(qRgb(0, 128, 0));
  QBrush brush(QPixmap(":/pics/cloth.png"));
  QRect NewRect = QRect(x1, y1, x2, y2);
  p.fillRect(NewRect, brush);
  p.end();
}


void DeskView::drawCard (const Card *card, int x, int y, bool opened, bool hilight) {
  char cCardName[16];
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
  QPixmap *bmp = GetImgByName(cCardName);
  if (!bmp) {
    //fprintf(stderr, "not found: [%s]\n", cCardName);
    bmp = GetImgByName("empty");
    if (!bmp) {
      ClearBox(x, y, x+CARDWIDTH, y+CARDHEIGHT);
      return;
    }
  }
  //fprintf(stderr, "found: [%s]\n", cCardName);
  QPainter p(mDeskBmp);
  p.drawPixmap(x, y, *bmp);
  p.end();
}


void DeskView::drawText (const QString str, int x, int y, QRgb textColor, QRgb outlineColor) {
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
  y += p.boundingRect(rect(), s).height();
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


void DeskView::MessageBox (const QString text, const QString caption) {
  QMessageBox mb(caption, text, QMessageBox::Information, QMessageBox::Ok | QMessageBox::Default, 0, 0);
  mb.exec();
}

eGameBid DeskView::selectBid (eGameBid lMove, eGameBid rMove) {
  Q_UNUSED(lMove)
  Q_UNUSED(rMove)
  //if (!formBid->exec()) qApp->quit();
  //connect(formBid, SIGNAL(rejected()), kpref, SLOT(slotAbortBid()));
  FormBid *formBid = FormBid::instance();
  formBid->_GamesType = undefined;
  do
  	formBid->exec();
  while (formBid->_GamesType == undefined);
  return formBid->_GamesType;
}

void DeskView::drawPlayerMessage (int player, const QString msg, bool dim)
{
  int x, y;
  switch (player) {
    case 1:
      x = -666;
      y = -(m_topBottomMargin+CARDHEIGHT+40);
      break;
    case 2:
      x = 30;
      y = 10;
      break;
    case 3:
      x = -30;
      y = 10;
      break;
    default:
      x = -666;
      y = -666;
      break;
  }
  drawMessageWindow(x, y, msg, dim);
}

void DeskView::drawMessageWindow (int x0, int y0, const QString msg, bool dim) {
  if (!mDeskBmp) return;
  QPainter p(mDeskBmp);
  // change suits to unicode chars
  QRgb textRGB(qRgb(0, 0, 0));
  QString s(msg);
  if (s.indexOf("\1d") >= 0 || s.indexOf("\1h") >= 0) textRGB = qRgb(255, 0, 0);
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
    if (x0 == -666)
      x0 = (width()-w)/2;
    else
      x0 += width()-w;
  }
  if (y0 < 0) {
    if (y0 == -666)
      y0 = (height()-h)/2;
    else
      y0 += height()-h;
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
  p.setPen(textRGB);
  p.drawText(boundR, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextExpandTabs, s);
  // done
  p.end();
}


void DeskView::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event)
  m_model->draw();
}


void DeskView::mouseMoveEvent (QMouseEvent *event) {
  m_model->currentPlayer()->highlightCard(event->x(), event->y());
}


void DeskView::mousePressEvent (QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    Player *plr = m_model->currentPlayer();
    plr->mClickX = event->x();
    plr->mClickY = event->y();
  }
}


void DeskView::keyPressEvent (QKeyEvent *event) {
    switch (event->key()) {
      case Qt::Key_Escape:
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Space:
//        Event = 2;
        event->accept();
        break;
      default: ;
    }
}


void DeskView::showEvent (QShowEvent *event)
{
  Q_UNUSED(event)
  delete mDeskBmp;
  mDeskBmp = new QPixmap(width(), height());
  ClearScreen();
}

void DeskView::paintEvent (QPaintEvent *event) {
  Q_UNUSED(event)
  //qDebug() << "paintEvent";
    //mDeskView->DesktopHeight = height()-HINTBAR_MAX_HEIGHT;
    //mDeskView->DesktopWidth = width();
    QPainter p;
    p.begin(this);
    p.drawPixmap(0, 0, *(mDeskBmp));
    p.end();
}

void DeskView::getLeftTop (int player, int & left, int & top)
{
  left = 0; top = 0;
  switch (player) {
    case 1:
      left = width()/4 + m_leftRightMargin; //(DesktopWidth - (width() / 2 - 2 * xBorder)) / 2;
      top = height() - m_topBottomMargin - CARDHEIGHT; //DesktopHeight - yBorder - CARDHEIGHT;//mDeskView->CardHeight;//-10;
      break;
    case 2:
      left = m_leftRightMargin;
      top = m_topBottomMargin;// + 20;
      break;
    case 3:
      left = width() - m_leftRightMargin;
      top = m_topBottomMargin;// + 20;
      break;
    default: ;
  }
}

// draw ingame card (the card that is in game, not in hand)
void DeskView::inGameCardLeftTop (int mCardNo, int &left, int &top)
{
  int x = width()/2, y = height()/2;
  switch (mCardNo) {
    case 0:
      x -= CARDWIDTH*2+8;
      y -= CARDHEIGHT/2;
      break;
    case 1:
      x -= CARDWIDTH/2;
      break;
    case 2:
      x -= CARDWIDTH;
      y -= CARDHEIGHT;
      break;
    case 3:
      y -= CARDHEIGHT;
      break;
    default: return;
  }
  left = x; top = y;
}

void DeskView::drawInGameCard (int mCardNo, const Card *card, bool closed)
{
  if (!card) return;
  int x, y;
  inGameCardLeftTop(mCardNo, x, y);
  drawCard(card, x, y, !closed, 0);
}

void DeskView::animateDeskToPlayer (int plrNo, Card *mCardsOnDesk[], bool doAnim)
{
  static const int steps = 10;
  const Card *cAni[4];
  int left, top;

//  Player *plr = player(plrNo);
//  if (!plr) return;
  getLeftTop(plrNo, left, top);
  if (plrNo == 3) left -= CARDWIDTH-4;

  for (int f = 0; f < 4; f++) {
    cAni[f] = mCardsOnDesk[f];
    mCardsOnDesk[f] = 0;
  }

  for (int f = doAnim?0:steps; f <= steps; f++) {
    m_model->draw(false);
    for (int c = 0; c <= 3; c++) {
      if (!cAni[c]) continue;
      int x, y;
      inGameCardLeftTop(c, x, y);
      x = x+((int)((double)(left-x)/steps*f));
      y = y+((int)((double)(top-y)/steps*f));
      drawCard(cAni[c], x, y, 1, 0);
    }
    aniSleep(20);
  }
}


void DeskView::drawPool () {
/*  QString sb, sm, slw, srw, tw;
  ShowBlankPaper(optMaxPool);
  for (int i = 1;i<=3;i++) {
    Player *plr = mDesktop->player(i);
    sb = plr->mScore.poolStr();
    sm = plr->mScore.mountainStr(7);
    slw = plr->mScore.leftWhistsStr(14);
    srw = plr->mScore.rightWhistsStr(14);
    tw = plr->mScore.whistsStr();
    showPlayerScore(i, sb, sm, slw, srw, tw);
  }
*/
  d_ptr->m_scorew->exec();
}

void DeskView::drawBidBoard()
{
    Player *plr1 = m_model->player(1);
    Player *plr2 = m_model->player(2);
    Player *plr3 = m_model->player(3);
    Q_ASSERT(plr1 && plr2 && plr3);
    drawBidsBmp(m_model->activePlayerNumber(), plr1->tricksTaken(), plr2->tricksTaken(), plr3->tricksTaken(), gCurrentGame);
}

bool DeskView::askWhistType ()
{
  const int ret = QMessageBox::question(this, tr("Choose whist type"),
    tr("Do you want to whist with opened cards?"),
    QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape);
  if (ret == QMessageBox::Yes)
    return false;
  else
    return true;
}
