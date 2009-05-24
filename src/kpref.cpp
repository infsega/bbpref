#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QMainWindow>

#include "kpref.h"
#include "desktop.h"
#include "deskview.h"
#include "player.h"

#include "formbid.h"
#include <time.h>


char *documentation; //see bottom this file
Kpref *kpref;


Kpref::Kpref () {
  //!!!setTitle("OpenPref");
  initMenuBar();
  nflAleradyPainting = nAllReadyHinting = WaitingForMouseUp = 0;
  mDeskView = 0;
  DeskTop = 0;
  setMouseTracking(true);

  QDesktopWidget *desk = QApplication::desktop();
  QRect dims(desk->availableGeometry(this));
  int w = dims.width()-60;
  w = 528*2;
  int h = dims.height()-120;
  h = 800;
  int x = dims.left()+(dims.width()-w)/2;
  int y = dims.top()+(dims.height()-h)/2;
  move(x, y);
  resize(w, h);
}


void Kpref::init () {
  //StatusBar1 = new QStatusBar(this);
  mDeskView = new TDeskView(width(), height());
  DeskTop = new PrefDesktop(mDeskView);
  connect(DeskTop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
  connect(mDeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
}


Kpref::~Kpref () {
  //delete StatusBar1;
  //!delete formBid;
  delete mDeskView;
  delete DeskTop;
}


void Kpref::adjustDesk () {
  if (mDeskView) mDeskView->ClearScreen();
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
  viewMenu->addAction(QIcon(QString(":/pics/paper.png")), "Score...", this, SLOT(slotShowScore()), Qt::CTRL+Qt::Key_P);

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


void Kpref::slotShowScore () {
  DeskTop->CloseBullet();
  DeskTop->nflShowPaper = 1;
  DeskTop->ShowPaper();
  DeskTop->mDeskView->mySleep(-1);
  //mDeskView->ClearScreen();
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
      delete mDeskView;
      mDeskView = new TDeskView();
      mDeskView -> PaintDevice = this; // &&&
      DeskTop = new PrefDesktop();
      DeskTop -> mDeskView = mDeskView;
      optMaxPool = newgame.QSpinBox_2 ->value();
      g61stalingrad =  (newgame.StalingradGame->isChecked() == true);
      g10vist = (newgame.TenVist->isChecked() == true);
      globvist = (newgame.VistNonBr->isChecked() == true);
      mDeskView -> DesktopHeight = height();
      mDeskView -> DesktopWidth = width();
      //mDeskView->ClearScreen();
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
    delete mDeskView;
    mDeskView = new TDeskView(width(), height());
    DeskTop = new PrefDesktop();
    DeskTop->mDeskView = mDeskView;
    optMaxPool = 10; //k8:!!!
    g61stalingrad = true; //k8:!!!
    g10vist = false; //k8:!!!
    globvist = true; //k8:!!!
    connect(DeskTop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
    connect(mDeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
    //mDeskView->ClearScreen();
  }
  //!!!fileMenu->setItemEnabled(nSaveGameID,TRUE);
  DeskTop->RunGame();
}


void Kpref::mouseMoveEvent (QMouseEvent *event) {
  if (nAllReadyHinting) return;
  nAllReadyHinting = 1;
  Player *plr = DeskTop->currentPlayer();
  if (plr) plr->HintCard(event->x(), event->y());
  nAllReadyHinting = 0;
}


void Kpref::mousePressEvent (QMouseEvent *event) {
  WaitingForMouseUp = 1;
  mDeskView->Event = 1;
  Player *plr = DeskTop->currentPlayer();
  if (plr) {
    plr->X = event->x();
    plr->Y = event->y();
  }
  WaitingForMouseUp = 0;
}


void  Kpref::keyPressEvent (QKeyEvent *event) {
  Q_UNUSED(event)
  if (mDeskView) mDeskView->Event = 1;
}


void Kpref::paintEvent (QPaintEvent *event) {
  Q_UNUSED(event)
  if (nflAleradyPainting) return;
  nflAleradyPainting = 1;
  if (mDeskView) {
    mDeskView->DesktopHeight = height();
    mDeskView->DesktopWidth = width();
  }
  if (mDeskView && mDeskView->mDeskBmp) {
    QPainter p;
    p.begin(this);
    p.drawPixmap(0, 0, *(mDeskView->mDeskBmp));
    p.end();
  }
  nflAleradyPainting = 0;
}


void Kpref::slotHelpAbout () {
  QMessageBox::about(this, "About...",
    "OpenPref\n"
    "developer version (0.1.1)\n"
    "http://gitorious.org/openprefqt4\n"
    "\n"
    "(c) 2004 Fedotov A.V.\n"
    "Based on kpref (c) Azarniy I.V.\n"
    "Qt4 port and many new bugs by Ketmar"
  );
}
