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

#include <QHash>
#include <QPixmap>
#include <QString>

#include <QEventLoop>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWidget>

#include "prfconst.h"

class Card;
class PrefModel;
class DeskViewPrivate;

class DeskView : public QWidget {
  Q_OBJECT
public:
  DeskView (QWidget * parent = 0, Qt::WindowFlags f = 0);
  ~DeskView ();

  PrefModel * model() { return m_model; }
  void setModel(PrefModel *model);

  void mySleep (int seconds);
  void aniSleep (int milliseconds);

  // Background repaint
  void ClearScreen ();
  void ClearBox (int x1, int y1, int x2, int y2);
  
  void drawCard (Card *card, int x, int y, bool opened, bool hilight);
  void drawText (const QString &str, int x, int y, QRgb textColor=qRgb(255,255,255), QRgb outlineColor=qRgb(0,0,0));
  void MessageBox (const QString &text, const QString &caption);
  //----------------------------- for human player
  eGameBid selectBid (eGameBid lMove, eGameBid rMove);
  void StatusBar (const QString &text);

  void drawPKeyBmp (bool show);
  void drawPool ();
  /* game:
   *  <0: none
   *  =0: misere
   * suit:
   *  =0: nt
   * plrAct: 0-3
   */
  void drawBidsBmp (int plrAct, int p0t, int p1t, int p2t, eGameBid game);

  void drawMessageWindow (int x0, int y0, const QString msg, bool dim=false);
  void drawPlayerMessage (int player, const QString msg, bool dim=false);
  void drawIMove ();

  void getLeftTop (int player, int & left, int & top);
  
  void drawInGameCard (int mCardNo, Card *card, bool closed);
  void animateDeskToPlayer (int plrNo, Card *mCardsOnDesk[], bool doAnim);

  bool loadCards ();
  void freeCards ();

public:
  int imoveX, imoveY;

protected:
  void  showEvent (QShowEvent *);
  void  paintEvent (QPaintEvent *);
  void  keyPressEvent (QKeyEvent *);
  void  mousePressEvent (QMouseEvent *);
  void  mouseMoveEvent (QMouseEvent *);
  void  resizeEvent(QResizeEvent *event);

private:
  void inGameCardLeftTop (int mCardNo, int &left, int &top);

  void drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy);
  void drawNumber (int x0, int y0, int n, bool red);
  void drawGameBid (eGameBid game);
  QPixmap *GetImgByName (const char *name);

private:
  friend class DeskViewPrivate;
  DeskViewPrivate * d_ptr;  
  PrefModel *m_model;
  QPixmap *mDeskBmp;
  QPixmap *mBidBmp;
  QPixmap *mIMoveBmp;
  QPixmap *mKeyBmp[2];
  QPixmap *mDigitsBmp;
  int bidBmpX, bidBmpY;
  int xBorder, yBorder;
  int CardWidht, CardHeight;
  QHash<QString, QPixmap *> cardI;
  QHash<int, QPixmap *> bidIcons;
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
