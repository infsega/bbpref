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

#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QTimer>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QVBoxLayout>

#include "deskview.h"
#include "desktop.h"

#include "baser.h"
#include "formbid.h"
#include "prfconst.h"
#include "player.h"
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
  if (mDeskView->optPrefClub == true) {
    mDeskView->drawPKeyBmp(true);
//    mDeskView->update();
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
};

bool DeskView::loadCards () {
  // load cards from resources
  QString deckPath;
  if (optPrefClub) {
  	deckPath = "prefclub/";
    CardWidth = 56;
    CardHeight = 67;
  }
  else {
  	deckPath = "classic/";
    CardWidth = 71;
    CardHeight = 96;
  }

  setMinimumWidth(CardWidth*14.42);
  if (CardHeight*6 > 570)
    setMinimumHeight(CardHeight*6);
  else
    setMinimumHeight(570);

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

  // done
  return true;
}


void DeskView::freeCards () {
  // erase card images
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
DeskView::DeskView (QWidget * parent, Qt::WindowFlags f) : QWidget(parent,f), d_ptr(new DeskViewPrivate(this)),
                                                           m_model(0), mDeskBmp(0),
 optDebugHands(false),
 optDealAnim(true),
 optTakeAnim(true),
 optPrefClub(false),
 m_backgroundColor(qRgb(0,128,0))
{
  setAttribute(Qt::WA_OpaquePaintEvent);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  readSettings();

  mDigitsBmp = new QPixmap(":/pics/digits/digits.png");
  mBidBmp = new QPixmap(":/pics/bidinfo.png");
  mKeyBmp[0] = new QPixmap(":/pics/presskey.png");
  QImage key1(":/pics/presskey.png");
  yellowize(&key1, qRgb(127, 127, 127));
  mKeyBmp[1] = new QPixmap(QPixmap::fromImage(key1));
  mIMoveBmp = new QPixmap(":/pics/imove.png");

  const bool result = loadCards();
  Q_ASSERT(result);

  m_leftRightMargin = 20;
  m_topBottomMargin = 20;
  mDeskBmp = new QPixmap(width(), height());
  ClearScreen();
  setMouseTracking(true);
  setAutoFillBackground(false);
}


DeskView::~DeskView () {
  writeSettings();

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

void DeskView::setBackgroundType(const int type)
{
  m_backgroundType = type;
  switch(m_backgroundType) {
    case 1:
      m_deskBackground = QPixmap(":/pics/cloth.png");
      break;
    case 2:
      m_deskBackground = QPixmap(":/pics/wood.jpg").scaled(size());
      break;
    default:
      m_deskBackground.setColor(m_backgroundColor);
      m_deskBackground.setStyle(Qt::SolidPattern);
      break;
  }
}

void DeskView::drawIMove (QPainter &p) {
  int x, y;
  switch (m_model->nCurrentStart.nValue) {
      case 1:
        x = width()/2-15;
        y = height() - m_topBottomMargin - CardHeight - 30;
        break;
      case 2:
        x = m_leftRightMargin+20;
        y = m_topBottomMargin + CardHeight + 20;
        break;
      case 3:
        x = width() - m_leftRightMargin - 20;
        y = m_topBottomMargin + CardHeight + 20;
        break;
      default:
        qDebug() << "Invalid nCurrentStart.nValue =" << m_model->nCurrentStart.nValue;
        x = y = -1;
        break;
  }
  p.drawPixmap(x, y, *mIMoveBmp);
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
  update(QRegion(4, height()-(i->height()+8), i->width(), i->height()));
}


void DeskView::drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy) {
  QRect tgt(x0, y0, 8, 14);
  QRect src(cx, cy, 8, 14);
  p.drawPixmap(tgt, *mDigitsBmp, src);
}


void DeskView::drawNumber (QPainter &p, int x0, int y0, int n, bool red) {
  //if (!mDeskBmp || n < 0 || n > 1024) return;
  char buf[12], *pd;
  sprintf(buf, "%i", n);
  pd = buf;
  while (*pd) {
    int d = (*pd++)-'0';
    int px = d*8, py = red?14:0;
    if (red) drawBmpChar(p, x0+1, y0+1, px, 0);
    drawBmpChar(p, x0, y0, px, py);
    x0 += 8;
  }
}


static int numWidth (int n) {
  if (n < 0 || n > 1024) return 0;
  char buf[16];
  sprintf(buf, "%i", n);
  return strlen(buf)*8;
}


void DeskView::drawGameBid (QPainter &p, eGameBid game) {
  Q_ASSERT(game == raspass || (game >= 61 && game <= 105));
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
        p.drawPixmap(bidBmpX+x, bidBmpY+y, *i);
      }
      break;
  }
}


void DeskView::mySleep (int seconds) {
  installEventFilter(d_ptr->m_efilter);

  if (seconds == 0)
  {
    drawPKeyBmp(false);
    removeEventFilter(d_ptr->m_efilter);
    return;
  }
  if (seconds > 0) {
    connect(d_ptr->m_timer, SIGNAL(timeout()), d_ptr->m_eloop, SLOT(quit()));
    d_ptr->m_timer->start(seconds*1000);
  } else if (seconds == -1) {
    connect(d_ptr->m_timer, SIGNAL(timeout()), d_ptr->m_eloop, SLOT(keyPicUpdate()));
    d_ptr->m_timer->start(1000);
  } else if (seconds < -1) {
    d_ptr->m_eloop->mIgnoreKey = true;
  }
  if (optPrefClub == true)
  drawPKeyBmp(seconds == -1);

  if (d_ptr->m_eloop->isRunning())
    d_ptr->m_eloop->exit();
  if(seconds >= 0)
    d_ptr->m_eloop->exec();
  else
    d_ptr->m_eloop->exec(QEventLoop::WaitForMoreEvents);
  removeEventFilter(d_ptr->m_efilter);
  disconnect(d_ptr->m_timer, SIGNAL(timeout()), 0, 0);

  if (seconds == -1) {
    drawPKeyBmp(false);
  }
}


void DeskView::aniSleep (int milliseconds, const QRegion & region) {
  d_ptr->m_eloop->mIgnoreMouse = true;
  d_ptr->m_eloop->mIgnoreKey = true;
  installEventFilter(d_ptr->m_efilter);
  connect(d_ptr->m_timer, SIGNAL(timeout()), d_ptr->m_eloop, SLOT(quit()));
  d_ptr->m_timer->start(milliseconds);

  if (region.isEmpty())
    repaint();
  else
    repaint(region);
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
  Q_ASSERT(mDeskBmp);
  QPainter p(mDeskBmp);
  QRect NewRect = QRect(x1, y1, x2, y2);
  p.fillRect(NewRect, m_deskBackground);
  p.end();
}


void DeskView::drawCard (const Card *card, int x, int y, bool opened, bool hilight) {
  char cCardName[16];
  cCardName[0] = 0;
  if (!card) {
    ClearBox(x, y, x+CardWidth, y+CardHeight);
    return;
  }

  //strcpy(cCardName, opened?"empty":"1000");
  if (opened) {
    Q_ASSERT(card->face() >= 7 && card->face() <= FACE_ACE && card->suit() >= 1 && card->suit() <= 4);
    sprintf(cCardName, "%s%i%i", hilight?"q":"", card->face(), card->suit());
  } else {
    sprintf(cCardName, "1000");
  }
  QPixmap *bmp = GetImgByName(cCardName);
  if (!bmp) return;
  QPainter p(mDeskBmp);
  p.drawPixmap(x, y, *bmp);
  p.end();
}


void DeskView::drawText (const QString & str, int x, int y, QRgb textColor, QRgb outlineColor) {
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


void DeskView::MessageBox (const QString & text, const QString & caption) {
  QMessageBox mb(caption, text, QMessageBox::Information, QMessageBox::Ok | QMessageBox::Default, 0, 0);
  mb.exec();
}

eGameBid DeskView::selectBid (eGameBid lMove, eGameBid rMove) {
  Q_UNUSED(lMove)
  Q_UNUSED(rMove)
  FormBid *formBid = FormBid::instance();
  formBid->_GamesType = undefined;
  do
  	formBid->exec();
  while (formBid->_GamesType == undefined);
  return formBid->_GamesType;
}

void DeskView::drawPlayerMessage (int player, const QString & msg, bool dim)
{
  if(msg.isEmpty())
    return;
  int x, y;
  switch (player) {
    case 1:
      x = -666;
      y = -(m_topBottomMargin+CardHeight+40);
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

void DeskView::drawMessageWindow (int x0, int y0, const QString & msg, bool dim) {
  Q_ASSERT(mDeskBmp);
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
  setBackgroundType(m_backgroundType);
  draw(false);
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
        event->accept();
        break;
      default: ;
    }
}


void DeskView::paintEvent (QPaintEvent *event) {
  QPainter p(this);
//  qDebug() << event->region().boundingRect();
  p.setClipRegion(event->region());
  p.drawPixmap(0, 0, *(mDeskBmp));
  p.end();
}

void DeskView::getLeftTop (int player, int & left, int & top)
{
  left = 0; top = 0;
  switch (player) {
    case 1:
      left = width()/4 + m_leftRightMargin;
      top = height() - m_topBottomMargin - CardHeight;
      break;
    case 2:
      left = m_leftRightMargin;
      top = m_topBottomMargin;
      break;
    case 3:
      left = width() - m_leftRightMargin;
      top = m_topBottomMargin;
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
      x -= CardWidth*2+8;
      y -= CardHeight/2;
      break;
    case 1:
      x -= CardWidth/2;
      break;
    case 2:
      x -= CardWidth;
      y -= CardHeight;
      break;
    case 3:
      y -= CardHeight;
      break;
    default: return;
  }
  left = x; top = y;
}

void DeskView::animateTrick (int plrNo, Card *mCardsOnDesk[])
{
  const int steps = 20;
  const Card *cAni[4];
  int left, top;

//  Player *plr = player(plrNo);
//  if (!plr) return;
  getLeftTop(plrNo, left, top);
  if (plrNo == 3) left -= CardWidth-4;

  for (int f = 0; f < 4; f++) {
    cAni[f] = mCardsOnDesk[f];
    mCardsOnDesk[f] = 0;
  }

  if(optTakeAnim) {
    draw(false);
    QRegion oldRegion;
    QPixmap cache = *mDeskBmp;
    for (int f = optTakeAnim?0:steps; f <= steps; f++) {
      QRegion newRegion;
      for (int c = 0; c <= 3; c++) {
        if (!cAni[c]) continue;
        int x, y;
        inGameCardLeftTop(c, x, y);
        x = x+((int)((double)(left-x)/steps*f));
        y = y+((int)((double)(top-y)/steps*f));
        drawCard(cAni[c], x, y, 1, 0);
        newRegion += QRegion(x, y, CardWidth, CardHeight);
      }
      aniSleep(200/steps, newRegion + oldRegion);
      *mDeskBmp = cache;
      oldRegion = newRegion;
    }
  }
  draw();
}


void DeskView::drawPool () {
//#ifdef MOBILE
//  d_ptr->m_scorew->showFullScreen();
//#endif
  d_ptr->m_scorew->exec();
//#endif
}

void DeskView::drawBidBoard()
{
    Q_ASSERT(mDeskBmp);
    const Player *plr1 = m_model->player(1);
    const Player *plr2 = m_model->player(2);
    const Player *plr3 = m_model->player(3);
    const int plrAct = m_model->activePlayerNumber();
    Q_ASSERT(plr1 && plr2 && plr3);
    QPixmap *i = mBidBmp;
    bidBmpX = width()-(i->width()+8);
    bidBmpY = height()-(i->height()+8);
    QPainter p(mDeskBmp);
    p.drawPixmap(bidBmpX, bidBmpY, *i);
  /*
   * p0t: 10,6 (left top)
   * p1t: 77,6 (right top)
   * p2t: 44,50 (bottom center)
   * bid: 44,34 (center)
  */
    // human
      const int wdt1 = numWidth(plr1->tricksTaken() >= 0);
      const int x1 = 44-(wdt1/2);
      drawNumber(p, bidBmpX+x1, bidBmpY+50, plr1->tricksTaken(), plrAct==1);
    // left-top ai
      drawNumber(p, bidBmpX+10, bidBmpY+6, plr2->tricksTaken(), plrAct==2);
    // right-top ai
      const int wdt2 = numWidth(plr3->tricksTaken());
      const int x2 = 77-wdt2;
      drawNumber(p, bidBmpX+x2, bidBmpY+6, plr3->tricksTaken(), plrAct==3);
    drawGameBid(p, m_model->gCurrentGame);
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

/// @todo Should be merged with paintEvent (and add method like invalidateCache)
void DeskView::draw (bool emitSignal) {
  ClearScreen();
  if(!m_model->mGameRunning)
    return;

  // repaint players
  for (int f = 1; f <= 3; f++) 
      m_model->player(f)->draw();

  // repaint in-game cards
  for (int f = 0; f <= 3; f++)
    if (m_model->cardOnDesk(f)) {
      int x, y;
      inGameCardLeftTop(f, x, y);
      drawCard(m_model->cardOnDesk(f), x, y, !m_model->mOnDeskClosed, 0);
    }

  // draw bidboard
  if (m_model->mPlayingRound) {
    drawBidBoard();
  }
  QPainter p(mDeskBmp);
  drawIMove(p);
  p.end();
  /// @todo Calculate region which really needs updating
  if (emitSignal) update();
}

void DeskView::longWait (const int n)
{
  if (optPrefClub)
    mySleep(-1);
  else
    mySleep(2*n);
}

void DeskView::readSettings()
{
    QSettings st;
    setBackgroundType(st.value("background_type", 1).toInt());
    m_backgroundColor = st.value("background_color", qRgb(0,128,0)).toUInt();
    optDealAnim = st.value("animdeal", true).toBool();
    optTakeAnim = st.value("animtake", true).toBool();
    optDebugHands = st.value("debughand", false).toBool();
    optPrefClub = st.value("prefclub", false).toBool();
}

void DeskView::writeSettings() const
{
    QSettings st;
    st.setValue("background_type", m_backgroundType);
    st.setValue("background_color", m_backgroundColor);
    st.setValue("prefclub", optPrefClub);
    st.setValue("animdeal", optDealAnim);
    st.setValue("animtake", optTakeAnim);
    st.setValue("debughand", optDebugHands);
}
