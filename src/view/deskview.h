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

//deskview.h
// all graphics function depend
#ifndef DESKVIEW_H
#define DESKVIEW_H

#include <QtCore/QHash>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QEventLoop>

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QRgb>
#include <QtGui/QWidget>

#include "prfconst.h"
#include "formbid.h"

class Card;
class PrefModel;
class DeskViewPrivate;

/**
 * @class DeskView deskview.h
 * @brief Widget for visualization of preferans game
 *
 * Provides a self-contained widget capabla of preferans game visualization.
 * Works in pair with PrefModel.
 */
class DeskView : public QWidget {
  Q_OBJECT
public:
  explicit DeskView (QWidget * parent = 0, Qt::WindowFlags f = 0);
  ~DeskView ();

  PrefModel * model() { return m_model; }
  void setModel(PrefModel *model);
  int backgroundType() const { return m_backgroundType; }
  void setBackgroundType(const int type);
  QRgb backgroundColor() const { return m_backgroundColor; }
  void setBackgroundColor(const QRgb color);
  int takeAnimQuality() const { return m_takeQuality; }
  void setTakeQuality(const int q) { m_takeQuality = q; }

  void readSettings();
  void writeSettings() const;

  void mySleep (int seconds);
  void aniSleep (int milliseconds, const QRegion & region = QRegion());

  // Background repaint
  void ClearScreen ();
  void ClearBox (int x1, int y1, int x2, int y2);

  void draw (bool emitSignal=true);
  void drawCard (const Card *card, int x, int y, bool opened, bool hilight);
  void drawText (const QString & str, int x, int y, QRgb textColor=qRgb(255,255,255), QRgb outlineColor=qRgb(0,0,0));
  void drawPool ();
  void drawBidBoard();
  void drawMessageWindow (int x0, int y0, const QString & msg, bool dim=false);
  void drawPlayerMessage (int player, const QString & msg, bool dim=false);

  void MessageBox (const QString & text, const QString & caption);
  //----------------------------- for human player
  eGameBid selectBid (BidDialog::ActiveButtons);

  void getLeftTop (int player, int & left, int & top);
  
  void animateTrick (int plrNo, Card *mCardsOnDesk[]);

  bool askWhistType ();
  void longWait (const int n);

  void reloadCards () { freeCards(); loadCards(); }

public:
  int imoveX, imoveY;
  bool optDebugHands;
  bool optDealAnim;
  bool optTakeAnim;
  bool optPrefClub;
  int CardWidth, CardHeight;

protected:
  void  paintEvent (QPaintEvent *);
  void  keyPressEvent (QKeyEvent *);
  void  mousePressEvent (QMouseEvent *);
  void  mouseMoveEvent (QMouseEvent *);
  void  resizeEvent(QResizeEvent *event);

private:
  void drawPKeyBmp (bool show);
  void inGameCardLeftTop (int mCardNo, int &left, int &top);
  void drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy);
  void drawNumber (QPainter &p, int x0, int y0, int n, bool red);
  void drawGameBid (QPainter &p, eGameBid game);
  void drawIMove (QPainter &p);

  void freeCards();
  bool loadCards ();

  QPixmap *GetImgByName (const char *name);

private:
  friend class DeskViewPrivate;
  friend class SleepEventLoop;
  DeskViewPrivate * d_ptr;  
  PrefModel *m_model;
  QPixmap *mDeskBmp;
  QPixmap *mBidBmp;
  QPixmap *mIMoveBmp;
  QPixmap *mKeyBmp[2];
  QPixmap *mDigitsBmp;
  int bidBmpX, bidBmpY;
  int m_leftRightMargin, m_topBottomMargin;
  QHash<QString, QPixmap *> cardI;
  QMap<int, QPixmap *> bidIcons;
  int m_backgroundType;
  QRgb m_backgroundColor;
  QBrush m_deskBackground;
  int m_takeQuality;
};



class SleepEventLoop : public QEventLoop {
  Q_OBJECT
public:
  SleepEventLoop (DeskView *aDeskView, QObject *parent=0) : QEventLoop(parent),
    mDeskView(aDeskView), mKeyPressed(false), mMousePressed(false), mMouseX(0), mMouseY(0),
    mIgnoreKey(false), mIgnoreMouse(false) { }

  void doEventKey (QKeyEvent *event);
  void doEventMouse (QMouseEvent *event);

public slots:
  void keyPicUpdate ();
  void quit();

public:
  DeskView *mDeskView;

  bool mKeyPressed; // exited due to mouse click?
  bool mMousePressed; // exited due to mouse click?
  int mMouseX, mMouseY; // where the click was?

  bool mIgnoreKey; // ignore key events?
  bool mIgnoreMouse; // ignore mouse events?
};

#endif
