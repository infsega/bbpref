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

#ifndef KPREF_H
#define KPREF_H

#include "desktop.h"
#include "deskview.h"

#include <QMainWindow>

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
  
  // Messages
  void MoveImpossible ();
  void HintMove ();
  bool WhistType ();

public slots:
  void forceRepaint ();

  void slotNewSingleGame ();
  void slotShowScore ();
  void slotFileOpen ();
  void slotFileSave ();
  void slotHelpAbout ();
  void slotRules ();
  void slotOptions ();
  void slotDeckChanged ();

  void slotQuit ();
  //void slotAbortBid ();

private:
  void saveOptions ();
  void loadOptions ();

  QAction *actNewGame;
  QAction *actFileOpen;
  QAction *actFileSave;
  QStatusBar * HintBar;
  //QLabel * Hint;

public:
  PrefDesktop *mDesktop;
  DeskView *mDeskView;
  void showHint(QString hint);
  void clearHint();

protected:
  void  paintEvent (QPaintEvent *);
  //void  mousePressEvent (QMouseEvent *);
  //void  mouseMoveEvent (QMouseEvent *);
  void  keyPressEvent (QKeyEvent *);
  void  closeEvent(QCloseEvent *event);
  //void  resizeEvent(QResizeEvent *event);
};

extern Kpref *kpref;


#endif
