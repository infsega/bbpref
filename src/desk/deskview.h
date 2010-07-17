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

#include <QApplication>
#include <QEventLoop>
#include <QHash>
#include <QImage>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QString>

#include "card.h"
#include "prfconst.h"

class DeskView : public QObject {
  Q_OBJECT
public:
  DeskView (int aW, int aH);
  ~DeskView ();

  void mySleep (int seconds);
  void aniSleep (int milliseconds);

  void ClearScreen ();
  void ClearBox (int x1, int y1, int x2, int y2);
  void drawCard (Card *card, int x, int y, bool opened, bool hilight);
  void drawText (const QString &str, int x, int y, QRgb textColor=qRgb(255,255,255), QRgb outlineColor=qRgb(0,0,0));
  void MessageBox (const QString &text, const QString &caption);
  void ShowBlankPaper (int optMaxPool);
  void showPlayerScore (int i, const QString &sb, const QString &sm, const QString &slv, const QString &srv, const QString &tv);
  //----------------------------- for human player
  eGameBid selectBid (eGameBid lMove, eGameBid rMove);
  void StatusBar (const QString &text);
  void drawRotatedText (QPainter &p, int x, int y, float angle, const QString &text);
  void drawRotatedText (QPainter &p, int x, int y, int width, int height, float angle, const QString &text);

  void drawPKeyBmp (bool show);
  /* game:
   *  <0: none
   *  =0: misere
   * suit:
   *  =0: nt
   * plrAct: 0-3
   */
  void drawBidsBmp (int plrAct, int p0t, int p1t, int p2t, eGameBid game);

  void drawMessageWindow (int x0, int y0, const QString &msg, bool dim=false);
  void drawIMove (int x, int y);

  void emitRepaint ();

  bool loadCards ();
  void freeCards ();

signals:
  void deskChanged ();

public:
  int Event;
  QImage *mDeskBmp;
  QImage *mBidBmp;
  QImage *mIMoveBmp;
  QImage *mKeyBmp[2];
  QImage *mDigitsBmp;

  int nSecondStartWait;
  int DesktopWidth, DesktopHeight;
  int CardWidht, CardHeight;
  int xBorder, yBorder;
  //int xLen,yLen;
  int xDelta, yDelta;

  int imoveX, imoveY;

private:
  void drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy);
  void drawNumber (int x0, int y0, int n, bool red);
  void drawGameBid (eGameBid game);

private:
  int bidBmpX, bidBmpY;

private:
  QImage *GetImgByName (const char *name);

private:
  QHash<QString, QImage *> cardI;
  QImage *bidIcons[106]; // wasted!
};


#endif
