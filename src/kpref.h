/***************************************************************************
                          kpref.h  -  description
                             -------------------
    begin                : Mon Mar  6 14:43:31 EET 2000
    copyright            : (C) 2000 by Azarniy I.V.
    email                : azarniy@usa.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

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
  void slotShowBollet();
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
  QStatusBar *StatusBar1;

protected:
  void  paintEvent (QPaintEvent *);
  void  mousePressEvent ( QMouseEvent * );
  void  mouseMoveEvent ( QMouseEvent * );
  void  keyPressEvent ( QKeyEvent * );
};

extern Kpref *kpref;


#endif
