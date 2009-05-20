/***************************************************************************
                          kpref.cpp  -  description
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
#include <QApplication>
#include <QMouseEvent>
#include <QMainWindow>

#include "kpref.h"
#include "desktop.h"
#include "deskview.h"
#include "gamer.h"

#include "qtformtorg.h"
#include "debug.h"
#include <time.h>


char *documentation; //see bottom this file
Kpref *kpref;


Kpref::Kpref () {
  //!!!setTitle("OpenPref");
  initMenuBar();
  nflAleradyPainting = nAllReadyHinting = WaitingForMouseUp = 0;
  DeskView = NULL;
  DeskTop = NULL;
  setMouseTracking(true);
}


void Kpref::init () {
  StatusBar1 = new QStatusBar(this);
  DeskView = new TDeskView(width(), height());
  DeskTop = new TDeskTop(DeskView);
  connect(DeskTop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
  connect(DeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
}


Kpref::~Kpref () {
  delete StatusBar1;
  //!delete Formtorg;
  delete DeskView;
  delete DeskTop;
}


void Kpref::adjustDesk () {
  if (DeskView) DeskView->ClearScreen();
  update();
}


void Kpref::initMenuBar () {
  fileMenu = menuBar()->addMenu("&Game");
  actNewGame = fileMenu->addAction(QIcon(QString(":/pics/newgame.png")), "&New game...", this, SLOT(slotNewSingleGame()), Qt::CTRL+Qt::Key_N);
  fileMenu->addSeparator();
  actFileOpen = fileMenu->addAction(QIcon(QString(":/pics/fileopen.png")),"&Open...", this, SLOT(slotFileOpen()), Qt::CTRL+Qt::Key_O);
  actFileSave = fileMenu->addAction(QIcon(QString(":/pics/filesave.png")),"&Save", this, SLOT(slotFileSave()), Qt::CTRL+Qt::Key_S);
  fileMenu->addSeparator();
  actQuit = fileMenu->addAction("Quit", qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
  actFileSave->setEnabled(false);

  viewMenu = menuBar()->addMenu("&Show");
  viewMenu->addAction(QIcon(QString(":/pics/paper.png")), "Score...", this, SLOT(slotShowBollet()), Qt::CTRL+Qt::Key_P);

  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu("&Help");
  helpMenu->addAction("About", this, SLOT(slotHelpAbout()), 0);
}


///////////////////////////////////////////////////////////////////////////////
// slots
///////////////////////////////////////////////////////////////////////////////
void Kpref::slotFileOpen () {
  QString fileName = QFileDialog::getOpenFileName(this, "Select saved game", "", "*.prf");
  if (!fileName.isEmpty())  {
    DeskTop->LoadGame(fileName);
    actFileSave->setEnabled(true);
    DeskTop->RunGame();
  }
}


void Kpref::slotFileSave () {
  if (DeskTop) {
    QString fn = QFileDialog::getSaveFileName(this, "Select file to save the current game", "", "*.prf");
    if (!fn.isEmpty()) DeskTop->SaveGame(fn);
  }
}


/*
void Kpref::slotFileQuit() {
  //quit();
}
*/


void Kpref::slotEndSleep () {
  //QEvent Event(Event_Timer);
  //qApp->postEvent(this,&Event );
}


void Kpref::slotShowBollet () {
  DeskTop->CloseBullet();
  DeskTop->nflShowPaper = 1;
  DeskTop->ShowPaper();
  DeskTop->DeskView->mySleep(0);
  DeskView->ClearScreen();
  DeskTop->nflShowPaper = 0;
  DeskTop->Repaint();
}


void Kpref::forceRepaint () {
  update();
}


void Kpref::slotNewSingleGame () {
/*
  qtnewgameu newgame(this,"NewSingleGame");
  if (  newgame.exec() ) {
    if (DeskTop) {
      delete DeskTop;
      delete DeskView;
      DeskView = new TDeskView();
      DeskView -> PaintDevice = this; // &&&
      DeskTop = new TDeskTop();
      DeskTop -> DeskView = DeskView;
      nBuletScore = newgame.QSpinBox_2 ->value();
      g61stalingrad =  (newgame.StalingradGame->isChecked() == true);
      g10vist = (newgame.TenVist->isChecked() == true);
      globvist = (newgame.VistNonBr->isChecked() == true);
      DeskView -> DesktopHeight = height();
      DeskView -> DesktopWidht = width();
      //DeskView->ClearScreen();
      if (newgame.flProtocol->isChecked () ) {
        QByteArray ba(newgame.ProtocolFileName->text().toUtf8());
        strcpy(DeskTop->ProtocolFileName, ba.constData());
        DeskTop -> flProtocol = 1;
        DeskTop -> OpenProtocol();
      }
    }
    //!!!fileMenu->setItemEnabled(nSaveGameID,TRUE);
    DeskTop -> RunGame();
  }
*/
  if (DeskTop) {
    delete DeskTop;
    delete DeskView;
    DeskView = new TDeskView(width(), height());
    DeskTop = new TDeskTop();
    DeskTop->DeskView = DeskView;
    nBuletScore = 10; //k8:!!!
    g61stalingrad = true; //k8:!!!
    g10vist = false; //k8:!!!
    globvist = true; //k8:!!!
    connect(DeskTop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
    connect(DeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
    //DeskView->ClearScreen();
  }
  //!!!fileMenu->setItemEnabled(nSaveGameID,TRUE);
  DeskTop->RunGame();
}


void Kpref::mouseMoveEvent (QMouseEvent *event) {
  if (nAllReadyHinting) return;
  nAllReadyHinting = 1;
  TGamer *Gamer = (TGamer *)DeskTop->Gamers->At(DeskTop->nCurrentMove.nValue);
  if (Gamer) Gamer->HintCard(event->x(), event->y());
  nAllReadyHinting = 0;
}


void Kpref::mousePressEvent (QMouseEvent *event) {
  WaitingForMouseUp = 1;
  DeskView->Event = 1;
  TGamer *Gamer = (TGamer *)DeskTop->Gamers->At(DeskTop->nCurrentMove.nValue);
  if (Gamer) {
    Gamer->X = event->x();
    Gamer->Y = event->y();
  }
  WaitingForMouseUp = 0;
}


void  Kpref::keyPressEvent (QKeyEvent *event) {
  Q_UNUSED(event)
  if (DeskView) DeskView->Event = 1;
}


void Kpref::paintEvent (QPaintEvent *event) {
  Q_UNUSED(event)
  if (nflAleradyPainting) return;
  nflAleradyPainting = 1;
  if (DeskView) {
    DeskView->DesktopHeight = height();
    DeskView->DesktopWidht = width();
  }
  if (DeskView && DeskView->mDeskBmp) {
    QPainter p;
    p.begin(this);
    p.drawPixmap(0, 0, *(DeskView->mDeskBmp));
    p.end();
  }
  nflAleradyPainting = 0;
}


void Kpref::slotHelpAbout () {
  QMessageBox::about(this, "About...",
    "OpenPref\n"
    "developer version (0.1.0)\n"
    "http://openpref.boom.ru/\n"
    "\n"
    "(c) 2004 Fedotov A.V.\n"
    "\n"
    "Based on kpref (c) Azarniy I.V."
  );
}
