#ifndef KPREF_H
#define KPREF_H

#include <QString>
#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QPixmap>
#include <QToolButton>
#include <QApplication>
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

public slots:
  void forceRepaint ();

  void slotNewSingleGame();
  void slotShowScore();
  void slotEndSleep();
  void slotFileOpen();
  void slotFileSave();
  void slotHelpAbout();

private:
  QMenu *netgameMenu;
  QMenu *fileMenu;
  QMenu *viewMenu;
  QMenu *helpMenu;

  QAction *actNewGame;
  QAction *actFileOpen;
  QAction *actFileSave;
  QAction *actQuit;

public:
  TDeskTop *DeskTop;
  TDeskView *DeskView;
  int WaitingForMouseUp;
  int nAllReadyHinting;
  int nflAleradyPainting;
  int nMultiGameIDConnect;
  int nMultiGameIDCreate;
  int nMultiGameInetID;
  //int nSaveGameID;
  //QStatusBar *StatusBar1;

protected:
  void  paintEvent (QPaintEvent *);
  void  mousePressEvent ( QMouseEvent * );
  void  mouseMoveEvent ( QMouseEvent * );
  void  keyPressEvent ( QKeyEvent * );
};

extern Kpref *kpref;


#endif
