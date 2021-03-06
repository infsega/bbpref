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

#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QPixmapCache>
#include <QtGui/QVBoxLayout>

#include "deskview.h"
#include "desktop.h"

#include "baser.h"
#include "prfconst.h"
#include "player.h"
#include "scorewidget.h"


// change white to yellow
static void yellowize (QImage& im, QRgb newColor=qRgb(255, 255, 0))
{
  for (int y = im.height()-1; y >= 0; y--)
  {
    for (int x = im.width()-1; x >= 0; x--)
    {
      QRgb p = im.pixel(x, y);
      if (qRed(p) == 255 && qGreen(p) == 255 && qBlue(p) == 255)
        im.setPixel(x, y, newColor);
    }
  }
}

namespace
{
  class SleepEventFilter : public QObject
  {
  public:
    SleepEventFilter (SleepEventLoop *eloop, QObject *parent=0) : QObject(parent) { mLoop = eloop; }

  protected:
    bool eventFilter (QObject *obj, QEvent *e);

  private:
    SleepEventLoop *mLoop;
    };

  bool SleepEventFilter::eventFilter (QObject *obj, QEvent *e)
  {
    Q_UNUSED(obj)
    //
    if (e->type() == QEvent::KeyPress)
    {
      QKeyEvent *event = static_cast<QKeyEvent *>(e);
      mLoop->doEventKey(event);
    }
    else if (e->type() == QEvent::MouseButtonPress)
    {
      QMouseEvent *event = static_cast<QMouseEvent *>(e);
      mLoop->doEventMouse(event);
    }
    return false;
  }
} // end of namespace

///////////////////////////////////////////////////////////////////////////////
void SleepEventLoop::doEventKey (QKeyEvent *event)
{
  if (mIgnoreKey)
    return;
  switch (event->key())
  {
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

void SleepEventLoop::doEventMouse (QMouseEvent *event)
{
  if (mIgnoreMouse)
    return;
  if (event->button() == Qt::LeftButton)
  {
    mMousePressed = true;
    mMouseX = event->x();
    mMouseY = event->y();
    quit();
  }
}

void SleepEventLoop::quit()
{
  mIgnoreKey = false;
  mIgnoreMouse = false;
  QEventLoop::quit();
}

class DeskViewPrivate: public QObject
{
public:
  DeskViewPrivate(DeskView *parent) : m_eloop(0), m_efilter(0), m_timer(0)
  {
    m_eloop = new SleepEventLoop(parent);
    m_efilter = new SleepEventFilter(m_eloop);
    m_timer = new QTimer(m_eloop);
    m_introAnimTimer = new QTimer(parent);
  }

  ~DeskViewPrivate()
  {
    m_efilter->deleteLater();
    m_eloop->deleteLater();
    m_timer->deleteLater();
    m_introAnimTimer->deleteLater();
  }

  SleepEventLoop *m_eloop;
  SleepEventFilter *m_efilter;
  QTimer *m_timer;
  QTimer *m_introAnimTimer;
};

bool DeskView::loadCards()
{
  // load cards from resources
  QString deckPath;
  deckPath = "classic/";
  CardWidth = 71;
  CardHeight = 96;

  setMinimumWidth(1024 - 60);
  setMinimumHeight(CardHeight * 6);
  //setMinimumWidth(CardWidth*14.42);
  //if (CardHeight*6 > 570)
  //  setMinimumHeight(CardHeight*6);
  //else
  //  setMinimumHeight(570);

  for (int face = 7; face <= 14; face++)
  {
    for (int suit = 1; suit <= 4; suit++)
    {
      QString fname, resname;
      resname.sprintf("%i%i", face, suit);
      fname.sprintf(":/pics/cards/%s%i%i.png", deckPath.toAscii().data(), face, suit);
      cardI[resname] = QPixmap(fname);
      // build highlighted image
      QImage im(fname);
      yellowize(im);
      resname.sprintf("q%i%i", face, suit);
      cardI[resname] = QPixmap::fromImage(im);
    }
  }
  // load cardback
  cardI["1000"] = QPixmap(":/pics/cards/"+deckPath+"1000.png");
  cardI["empty"] = QPixmap(":/pics/cards/"+deckPath+"empty.png");
  // load bid icons
  for (int f = 6; f <= 10; f++)
  {
    for (int s = 1; s <= 5; s++)
    {
      QString fname;
      fname.sprintf(":/pics/bids/s%i%i.png", f, s);
      bidIcons[f*10+s] = QPixmap(fname);
    }
  }
  cardI.squeeze();

  // done
  return true;
}


void DeskView::freeCards()
{
  // erase card images
  bidIcons.clear();
  cardI.clear();
}

///////////////////////////////////////////////////////////////////////////////
DeskView::DeskView (QWidget * parent, Qt::WindowFlags f)
    : QWidget(parent,f)
    , optDebugHands(false)
    , optDealAnim(true)
    , optTakeAnim(true)
    , d_ptr(new DeskViewPrivate(this))
    , m_model(NULL)
    , mBidBmp(":/pics/bidinfo.png")
    , mIMoveBmp(":/pics/imove.png")
    , mDigitsBmp(":/pics/digits/digits.png")
    , mCandelabriumBmp(":/pics/candelabrium.png")
    , m_backgroundColor(qRgb(0,128,0))
    , m_takeQuality(2)
    , m_firePos(0)
{
  setAttribute(Qt::WA_OpaquePaintEvent);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  readSettings();

  mKeyBmp[0] = QPixmap(":/pics/presskey.png");
  QImage key1(":/pics/presskey.png");
  yellowize(key1, qRgb(127, 127, 127));
  mKeyBmp[1] = QPixmap::fromImage(key1);

  bool ok = loadCards();
  Q_ASSERT(ok && "loadCards()");
  Q_UNUSED(ok);

  m_leftRightMargin = 20;
  m_topBottomMargin = 20;
  mDeskBmp = QPixmap(width(), height());
  ClearScreen();
  setMouseTracking(true);
  setAutoFillBackground(false);

  d_ptr->m_introAnimTimer->start(10);
  connect(d_ptr->m_introAnimTimer, SIGNAL(timeout()), SLOT(introAnimation()));
}

DeskView::~DeskView()
{
  writeSettings();
  freeCards();
}

void DeskView::setModel(PrefModel *desktop)
{
  m_model = desktop;
  d_ptr = new DeskViewPrivate(this);
}

void DeskView::setBackgroundType(const int type)
{
  m_backgroundType = type;
  switch(m_backgroundType)
  {
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

void DeskView::setBackgroundColor(const QRgb color)
{
  m_backgroundColor = color;
  if (m_backgroundType == 0)
  {
    m_deskBackground.setColor(m_backgroundColor);
    m_deskBackground.setStyle(Qt::SolidPattern);
  }
}

void DeskView::drawIMove (QPainter &p)
{
  int x = 0, y = 0;
  Q_ASSERT(m_model->nCurrentStart.nValue >= 1 && m_model->nCurrentStart.nValue <= 3);
  switch (m_model->nCurrentStart.nValue)
  {
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
  }
  p.drawPixmap(x, y, mIMoveBmp);
}

void DeskView::drawPKeyBmp (bool show)
{
  static int phase = 0;
  if (!mDeskBmp) return;
  QPixmap& img = mKeyBmp[phase];
  if (show)
  {
    QPainter p(&mDeskBmp);
    p.drawPixmap(4, height()-(img.height() + 8), img);
    p.end();
    phase = 1 - phase;
  }
  else
    ClearBox(4, height()-(img.height() + 8), img.width(), img.height());
  update(QRegion(4, height()-(img.height() + 8), img.width(), img.height()));
}

void DeskView::drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy)
{
  QRect tgt(x0, y0, 8, 14);
  QRect src(cx, cy, 8, 14);
  p.drawPixmap(tgt, mDigitsBmp, src);
}

void DeskView::drawNumber (QPainter &p, int x0, int y0, int n, bool red)
{
  char buf[12], *pd;
  sprintf(buf, "%i", n);
  pd = buf;
  while (*pd)
  {
    int d = (*pd++)-'0';
    int px = d*8, py = red?14:0;
    if (red) drawBmpChar(p, x0+1, y0+1, px, 0);
    drawBmpChar(p, x0, y0, px, py);
    x0 += 8;
  }
}

static int numWidth (int n)
{
  if (n < 0 || n > 1024)
    return 0;
  char buf[16];
  sprintf(buf, "%i", n);
  return strlen(buf)*8;
}

void DeskView::drawGameBid(QPainter &p, eGameBid game)
{
  Q_ASSERT(game == raspass || game == withoutThree || (game >= 61 && game <= 105));
  int x, y;
  switch (game)
  {
  case raspass:
    drawBmpChar(p, bidBmpX+44-4, bidBmpY+34-7, 80, 0);
    break;
  case g86: // misere
    drawBmpChar(p, bidBmpX+44-4, bidBmpY+34-7, 80, 14);
    break;
  default:
    if ( bidIcons.contains( (int)game ) )
    {
      QPixmap& img = bidIcons[(int)game];
      x = 44-(img.width()/2)+3;
      y = 34-(img.height()/2)-1;
      p.drawPixmap(bidBmpX+x, bidBmpY+y, img);
    }
    break;
  }
}

void DeskView::mySleep (int seconds)
{
  installEventFilter(d_ptr->m_efilter);

  if (seconds == 0)
  {
    drawPKeyBmp(false);
    removeEventFilter(d_ptr->m_efilter);
    return;
  }
  if (seconds > 0)
  {
    connect(d_ptr->m_timer, SIGNAL(timeout()), d_ptr->m_eloop, SLOT(quit()));
    d_ptr->m_timer->start(seconds*1000);
  }
  else if (seconds == -1)
    d_ptr->m_timer->start(1000);
  else if (seconds < -1)
    d_ptr->m_eloop->mIgnoreKey = true;

  if (d_ptr->m_eloop->isRunning())
    d_ptr->m_eloop->exit();
  if(seconds >= 0)
    d_ptr->m_eloop->exec();
  else
    d_ptr->m_eloop->exec(QEventLoop::WaitForMoreEvents);
  removeEventFilter(d_ptr->m_efilter);
  disconnect(d_ptr->m_timer, SIGNAL(timeout()), 0, 0);

  if (seconds == -1)
    drawPKeyBmp(false);
}

void DeskView::aniSleep (int milliseconds, const QRegion & region)
{
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

void DeskView::ClearScreen()
{
  if (mDeskBmp.width() != width() || mDeskBmp.height() != height())
    mDeskBmp = QPixmap(width(), height());
  ClearBox(0, 0, width(), height());
}

void DeskView::ClearBox (int x1, int y1, int x2, int y2)
{
  QPainter p(&mDeskBmp);
  p.fillRect(x1, y1, x2, y2, m_deskBackground);
}

void DeskView::drawCard (const Card *card, int x, int y, bool opened, bool hilight)
{
  if (!card)
  {
    ClearBox(x, y, x+CardWidth, y+CardHeight);
    return;
  }

  QString cardName;
  if (opened)
  {
    Q_ASSERT(card->face() >= 7 && card->face() <= FACE_ACE && card->suit() >= 1 && card->suit() <= 4);
    cardName.sprintf("%s%i%i", hilight ? "q" : "", card->face(), card->suit());
  }
  else
    cardName = "1000";
  Q_ASSERT(cardI.contains(cardName));

  QPainter p(&mDeskBmp);
  p.drawPixmap(x, y, cardI[cardName]);
  p.end();
}


void DeskView::drawOutlinedText (const QString & str, int x, int y, QRgb textColor, QRgb outlineColor)
{
  QString s(str);
  s.replace("\1s", QChar((ushort)0x2660));
  s.replace("\1c", QChar((ushort)0x2663));
  s.replace("\1d", QChar((ushort)0x2666));
  s.replace("\1h", QChar((ushort)0x2665));
  s = s.trimmed();

  QPainter p(&mDeskBmp);
  p.setPen(outlineColor);
  QBrush b(p.brush());
  b.setColor(outlineColor);
  p.setBrush(b);
  y += p.boundingRect(rect(), s).height();
  for (int dy = -1; dy <= 1; dy++)
    for (int dx = -1; dx <= 1; dx++)
      if (dx || dy)
          p.drawText(x+dx, y+dy, s);
  p.setPen(textColor);
  b.setColor(textColor);
  p.setBrush(b);
  p.drawText(x, y, s);
  p.end();
}

void DeskView::MessageBox(const QString & text, const QString & caption)
{
  QMessageBox mb(caption, text, QMessageBox::Information, QMessageBox::Ok | QMessageBox::Default, 0, 0);
  mb.exec();
}

eGameBid DeskView::selectBid (BidDialog::ActiveButtons buttons)
{
  BidDialog *bidDialog = BidDialog::instance();
  bidDialog->_GamesType = undefined;
  bidDialog->setActiveButtons(buttons);
  bidDialog->show();
  QEventLoop l(this);
  connect(bidDialog, SIGNAL(accepted()), &l, SLOT(quit()));
  l.exec();
  Q_ASSERT(bidDialog->_GamesType != undefined);
  return bidDialog->_GamesType;
}

void DeskView::drawPlayerMessage (int player, const QString & msg, bool dim, bool useCache)
{
  if(msg.isEmpty())
    return;
  QPainter painter(&mDeskBmp);
  QPixmap msgBubble;
  const QString key = msg + (dim ? "&&d" : "");

  // Find if we've already rendered it (short circuit prevents useless search)
  if(!useCache || !QPixmapCache::find(key, msgBubble))
  {
    msgBubble = drawMessageWindow(msg, dim);
    QPixmapCache::insert(key, msgBubble);
  }
  int x, y;
  switch (player)
  {
    case 1:
      x = (width()-msgBubble.width())/2;
      y = height()-msgBubble.height()-(m_topBottomMargin+CardHeight+40);
      break;
    case 2:
      x = 30;
      y = 10;
      break;
    case 3:
      x = width()-msgBubble.width()-30;
      y = 10;
      break;
    default:
      x = (width()-msgBubble.width())/2;
      y = (height()-msgBubble.height())/2;
      break;
  }
  painter.drawPixmap(x, y, msgBubble);
}

QPixmap DeskView::drawMessageWindow (const QString & msg, bool dim)
{
  // change suits to unicode chars
  QRgb textRGB(qRgb(0, 0, 0));
  QString s(msg);
  if (s.indexOf("\1d") >= 0 || s.indexOf("\1h") >= 0)
    textRGB = qRgb(255, 0, 0);
  s.replace("\1s", QChar((ushort)0x2660));
  s.replace("\1c", QChar((ushort)0x2663));
  s.replace("\1d", QChar((ushort)0x2666));
  s.replace("\1h", QChar((ushort)0x2665));
  s = s.trimmed();
  // now i want to know text extents
  //    QRect boundR = painter.boundingRect(rect(), Qt::AlignLeft | Qt::AlignTop | Qt::TextExpandTabs, msg);
  QRect boundR = fontMetrics().boundingRect(rect(), Qt::AlignLeft | Qt::AlignTop | Qt::TextExpandTabs, msg);
  // window size
  // .|.....text.....|.
  int w = boundR.width()+16, h = boundR.height()+16;
  QPixmap msgBubble = QPixmap(w, h);
  msgBubble.fill(Qt::transparent);
  QPainter p(&msgBubble);
  // draw shadow
  QBrush brush(qRgba(0, 0, 0, 128));
  QRect winR(4, 0, w-4, h-8);
  winR.adjust(-4, 8, -4, 8);
  p.fillRect(winR, brush);
  winR.adjust(4, -8, 4, -8);
  // draw window
  brush.setColor(qRgb(dim?164:255, dim?164:255, dim?164:255));
  p.fillRect(winR, brush);
  // draw frame
  p.setPen(qRgb(0, 0, 0));
  p.drawRect(5, 1, w-7, h-11);
  // draw text
  boundR.adjust(10, 4, 10, 4);
  p.setPen(textRGB);
  p.drawText(boundR, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextExpandTabs, s);

  //  painter.drawPixmap(x0, y0, msgBubble);
  return msgBubble;
}


void DeskView::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event)
  setBackgroundType(m_backgroundType);
  draw(false);
  qDebug() << "DeskView " << width() << " " << height();
}

void DeskView::mouseMoveEvent (QMouseEvent *event)
{
  m_model->currentPlayer()->highlightCard(event->x(), event->y());
}

void DeskView::mousePressEvent (QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    Player *plr = m_model->currentPlayer();
    plr->mClickX = event->x();
    plr->mClickY = event->y();
  }
}

void DeskView::keyPressEvent (QKeyEvent *event)
{
  switch (event->key())
  {
  case Qt::Key_Escape:
  case Qt::Key_Enter:
  case Qt::Key_Return:
  case Qt::Key_Space:
    event->accept();
    break;
  }
}

void DeskView::paintEvent (QPaintEvent *event)
{
  QPainter p(this);
  p.setClipRegion(event->region());
  p.drawPixmap(0, 0, mDeskBmp);
}

void DeskView::drawIntro()
{
  const int px[] = {600-150+29, 600-150+97, 600-150+124, 600-150+166, 600-150+222};
  const int py[] = { 20-125+78, 20-125+91,   20-125+35,   20-125+62,   20-125+68};
  QPainter p(&mDeskBmp);
  p.drawPixmap( 600, 20, mCandelabriumBmp );
  for( int i = 0; i < 5; i++)
  {
    int idx = i * 30 + m_firePos;
    idx = (idx % 150) + 1;
    QString name;
    name.sprintf(":/pics/flame/%04d.png", idx);
    p.drawPixmap( px[i], py[i], QPixmap(name) );
  }
}

void DeskView::introAnimation()
{
  if (!m_model || m_model->mGameRunning)
    return;
  m_firePos = (m_firePos + 1) % 150;
  draw();
}

const int PLAYER_OFFSET = 100;

void DeskView::getPlayerCardRoom(int player, int& left, int& right, int& top)
{
  left = 0; top = 0;
  switch (player)
  {
    case 1:
      left = PLAYER_OFFSET;
      right = width() - PLAYER_OFFSET;
      top = height() - m_topBottomMargin - CardHeight;
      break;
    case 2:
      left = m_leftRightMargin;
      right = (width() - m_leftRightMargin) / 2;
      top = m_topBottomMargin;
      break;
    case 3:
      left = (width() + m_leftRightMargin) / 2;
      right = width() - m_leftRightMargin;
      top = m_topBottomMargin;
      break;
    default: ;
  }
}

void DeskView::getPlayerTrickPos(int player, int& left, int& top)
{
  left = 0; top = 0;
  switch (player)
  {
    case 1:
      left = width() - CardWidth;
      top = height();
      break;
    case 2:
      left = -CardHeight;
      top = m_topBottomMargin + CardHeight;
      break;
    case 3:
      left = width() + CardHeight;
      top = m_topBottomMargin + CardHeight;
      break;
    default: ;
  }
}

// draw ingame card (the card that is in game, not in hand)
void DeskView::inGameCardLeftTop (int mCardNo, int &left, int &top)
{
  int x = width()/2, y = height()/2;
  switch (mCardNo)
  {
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
    default:
      return;
  }
  left = x; top = y;
}

void DeskView::animateTrick(int plrNo, const QCardList & cards)
{
  qDebug() << "Animate trick";
  const int steps = 5 * (2 << m_takeQuality);
  int left, top;

  getPlayerTrickPos(plrNo, left, top);
  if (plrNo == 3)
      left -= CardWidth - 4;

  if (optTakeAnim)
  {
    draw(false);
    QRegion oldRegion;
    QPixmap cache = mDeskBmp;
    for (int f = optTakeAnim ? 0 : steps; f <= steps; f++)
    {
      QRegion newRegion;
      for (int c = 0; c < cards.size(); c++)
      {
        if (!cards.at(c))
            continue;
        int x, y;
        inGameCardLeftTop(c, x, y);
        x += (left- x) * f / steps;
        y += (top - y) * f / steps;
        drawCard(cards.at(c), x, y, 1, 0);
        newRegion += QRegion(x, y, CardWidth, CardHeight);
      }
      aniSleep(200/steps, newRegion + oldRegion);
      mDeskBmp = cache;
      oldRegion = newRegion;
    }
  }
  draw();
}

void DeskView::animateCommunityCardTrick (int plrNo, const QCardList& cards)
{
  qDebug() << "animate community card trick";
  Player* player = m_model->player(plrNo);

  QMap<Card*, QPoint> finalPos;
  foreach( Card* pCard, cards )
  {
    int x, y;
    player->getCardPos(pCard, x, y);
    finalPos[pCard] = QPoint(x, y);
  }

  if (optTakeAnim)
  {
    draw(false);
    QRegion oldRegion;
    QPixmap cache = mDeskBmp;
    const int steps = 5 * (2 << m_takeQuality);
    for (int f = optTakeAnim ? 0 : steps; f <= steps; f++)
    {
      QRegion newRegion;
      for (int c = 0; c < cards.size(); c++)
      {
        Card* pCard = cards.at(c);
        if (!pCard)
            continue;
        int x, y;
        inGameCardLeftTop(c, x, y);
        QPoint pt = finalPos[pCard];
        x += (pt.x() - x) * f / steps;
        y += (pt.y() - y) * f / steps;
        drawCard(pCard, x, y, 1, 0);
        newRegion += QRegion(x, y, CardWidth, CardHeight);
      }
      aniSleep(200/steps, newRegion + oldRegion);
      mDeskBmp = cache;
      oldRegion = newRegion;
    }
  }
  player->mDealedCards.clear();
  draw();
}

void DeskView::drawPool()
{
  ScoreWidget scorew(m_model, this);
  scorew.exec();
}

void DeskView::drawBidBoard()
{
  const Player *plr1 = m_model->player(1);
  const Player *plr2 = m_model->player(2);
  const Player *plr3 = m_model->player(3);
  const int plrAct = m_model->activePlayerNumber();
  Q_ASSERT(plr1 && plr2 && plr3);
  bidBmpX = width()-(mBidBmp.width()+8);
  bidBmpY = height()-(mBidBmp.height()+8);
  QPainter p(&mDeskBmp);
  p.drawPixmap(bidBmpX, bidBmpY, mBidBmp);
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
  drawGameBid(p, m_model->currentGame());
}

bool DeskView::askWhistType()
{
  const int ret = QMessageBox::question(this, tr("Choose whist type"),
    tr("Do you want to whist with opened cards?"),
    QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape);
  return (ret == QMessageBox::No);
}

bool DeskView::askConfirmDrop ()
{
  const int ret = QMessageBox::question(this, tr("Confirm your drop"),
    tr("Do you confirm your drop?"),
    QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape);
  return (ret == QMessageBox::Yes);
}

/// @todo Should be merged with paintEvent (and add method like invalidateCache)
void DeskView::draw (bool emitSignal)
{
  ClearScreen();
  if (m_model->mGameRunning)
  {
    // repaint players
    for (int f = 1; f <= 3; f++)
      m_model->player(f)->draw();

    // repaint in-game cards
    for (int f = 0; f <= 3; f++)
    {
      if (m_model->cardOnDesk(f))
      {
        int x, y;
        inGameCardLeftTop(f, x, y);
        drawCard(m_model->cardOnDesk(f), x, y, !m_model->mOnDeskClosed, 0);
      }
    }

    // draw bidboard
    if (m_model->mPlayingRound)
      drawBidBoard();

    QPainter p(&mDeskBmp);
    drawIMove(p);
    p.end();
  }
  else
    drawIntro();

  /// @todo Calculate region which really needs updating
  if (emitSignal)
      update();
}

void DeskView::longWait (const int n)
{
  mySleep(2*n);
}

void DeskView::readSettings()
{
  QSettings st;
  setBackgroundType(st.value("background_type", 1).toInt());
  m_backgroundColor = st.value("background_color", qRgb(0,128,0)).toUInt();
  optDealAnim       = st.value("animdeal", true).toBool();
  optTakeAnim       = st.value("animtake", true).toBool();
  m_takeQuality     = st.value("takequality", 2).toInt();
  optDebugHands     = st.value("debughand", false).toBool();
}

void DeskView::writeSettings() const
{
  QSettings st;
  st.setValue("background_type",  m_backgroundType);
  st.setValue("background_color", m_backgroundColor);
  st.setValue("animdeal",         optDealAnim);
  st.setValue("animtake",         optTakeAnim);
  st.setValue("debughand",        optDebugHands);
  st.setValue("takequality",      m_takeQuality);
}
