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


class DeskView;
class SleepEventLoop;


class SleepEventFilter : public QObject {
  Q_OBJECT
public:
  SleepEventFilter (SleepEventLoop *eloop, QObject *parent=0) : QObject(parent) { mLoop = eloop; }

protected:
  bool eventFilter (QObject *obj, QEvent *e);

private:
  SleepEventLoop *mLoop;
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

public:
  DeskView *mDeskView;

  bool mKeyPressed; // exited due to mouse click?
  bool mMousePressed; // exited due to mouse click?
  int mMouseX, mMouseY; // where the click was?

  bool mIgnoreKey; // ignore key events?
  bool mIgnoreMouse; // ignore mouse events?
};


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
  bool loadCards ();
  void freeCards ();
  QImage *GetXpmByNameI (const char *name);

private:
  QHash<QString, QImage *> cardI;
  QImage *bidIcons[106]; // wasted!
};


#endif
