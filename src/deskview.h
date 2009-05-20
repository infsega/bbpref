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
  void drawCard (TCard *card, int x, int y, bool opened, bool hilight);
  void drawText (const QString &str, int x, int y, QRgb textColor=qRgb(255,255,255), QRgb outlineColor=qRgb(0,0,0));
  void MessageBox (const QString &text, const QString &caption);
  void ShowBlankPaper (int nBuletScore);
  void showPlayerScore (int i, const QString &sb, const QString &sm, const QString &slv, const QString &srv, const QString &tv);
  //----------------------------- for THuman gamer
  TGamesType makemove (TGamesType lMove,TGamesType rMove);
  void StatusBar (const QString &text);
  void drawRotatedText (QPainter &p, int x, int y, float angle, const QString &text);

  void emitRepaint ();

signals:
  void deskChanged ();

public:
  int Event;
  QPixmap *mDeskBmp;

  int nSecondStartWait;
  int DesktopWidht, DesktopHeight;
  int CardWidht, CardHeight;
  int xBorder, yBorder;
  //int xLen,yLen;
  int xDelta, yDelta;
};


#endif
