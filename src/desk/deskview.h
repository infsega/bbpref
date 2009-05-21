//deskview.h
// all graphics function depend
#ifndef DESKVIEW_H
#define DESKVIEW_H

#include <QString>
#include <QMainWindow>
#include <QPixmap>
#include <QApplication>
#include <QMessageBox>
#include <QImage>
#include <QPainter>

#include "prfconst.h"
#include "card.h"


class TDeskView : public QObject {
  Q_OBJECT
public:
  TDeskView (int aW, int aH);
  ~TDeskView ();

  void mySleep (int seconds);

  void ClearScreen ();
  void ClearBox (int x1, int y1, int x2, int y2);
  void drawCard (Card *card, int x, int y, bool opened, bool hilight);
  void drawText (const QString &str, int x, int y, QRgb textColor=qRgb(255,255,255), QRgb outlineColor=qRgb(0,0,0));
  void MessageBox (const QString &text, const QString &caption);
  void ShowBlankPaper (int nBuletScore);
  void showPlayerScore (int i, const QString &sb, const QString &sm, const QString &slv, const QString &srv, const QString &tv);
  //----------------------------- for human player
  eGameBid makemove (eGameBid lMove, eGameBid rMove);
  void StatusBar (const QString &text);
  void drawRotatedText (QPainter &p, int x, int y, float angle, const QString &text);

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

  void emitRepaint ();

signals:
  void deskChanged ();

public:
  int Event;
  QPixmap *mDeskBmp;
  QImage *mBidBmp;
  QImage *mKeyBmp[2];
  QImage *mDigitsBmp;

  int nSecondStartWait;
  int DesktopWidht, DesktopHeight;
  int CardWidht, CardHeight;
  int xBorder, yBorder;
  //int xLen,yLen;
  int xDelta, yDelta;

private:
  void drawBmpChar (QPainter &p, int x0, int y0, int cx, int cy);
  void drawNumber (int x0, int y0, int n, bool red);
  void drawGameBid (eGameBid game);

private:
  int bidBmpX, bidBmpY;
};


#endif
