#ifndef KPREF_H
#define KPREF_H

#include <QString>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QPixmap>
#include <QToolButton>
#include <QApplication>
#include <QLabel>
#include <QStatusBar>
#include <QWhatsThis>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>

//#include <pthread.h>

#include "desktop.h"
#include "deskview.h"


class Kpref : public QMainWindow {
  Q_OBJECT

public:
  Kpref();
  ~Kpref();
  void init();
  void initMenuBar();
  //QPainter *p;
  //pthread_t thread;
  //int flthread;

  void adjustDesk ();
  
  // HintBar actions
  void MoveImpossible ();
  void HintMove ();

public slots:
  void forceRepaint ();

  void slotNewSingleGame ();
  void slotShowScore ();
  void slotFileOpen ();
  void slotFileSave ();
  void slotHelpAbout ();
  void slotRules ();
  void slotOptions ();

  void slotAbort ();
  void slotAbortBid ();

private:
  void saveOptions ();
  void loadOptions ();

private:
  QMenu *netgameMenu;
  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;

  QAction *actNewGame;
  QAction *actFileOpen;
  QAction *actFileSave;
  QAction *actQuit;
  QAction *actOptions;
  QAction *actAboutQt;

public:
  PrefDesktop *mDesktop;
  DeskView *mDeskView;
  bool mWaitingMouseUp;
  bool mInMouseMoveEvent;
  bool mInPaintEvent;
  QStatusBar * HintBar;
  QLabel * Hint;

protected:
  void  paintEvent (QPaintEvent *);
  void  mousePressEvent (QMouseEvent *);
  void  mouseMoveEvent (QMouseEvent *);
  void  keyPressEvent (QKeyEvent *);
};

extern Kpref *kpref;


#endif
