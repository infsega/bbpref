#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QMainWindow>
#include <QSettings>

#include "kpref.h"

#include "desktop.h"
#include "deskview.h"
#include "formbid.h"
#include "optform.h"
#include "player.h"


//char *documentation; //see bottom this file
Kpref *kpref;


Kpref::Kpref () {
  //!!!setTitle("OpenPref");
  initMenuBar();
  mInPaintEvent = mInMouseMoveEvent = mWaitingMouseUp = false;
  mDeskView = 0;
  mDesktop = 0;
  setMouseTracking(true);

  loadOptions();

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
  mDeskView = new DeskView(width(), height());
  mDesktop = new PrefDesktop(mDeskView);
  connect(mDesktop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
  connect(mDeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
}


Kpref::~Kpref () {
  //delete StatusBar1;
  //!delete formBid;
  delete mDeskView;
  delete mDesktop;
}


void Kpref::adjustDesk () {
  if (mDeskView) mDeskView->ClearScreen();
  update();
}


void Kpref::initMenuBar () {
  fileMenu = menuBar()->addMenu("&Game");
  actNewGame = fileMenu->addAction(QIcon(QString(":/pics/newgame.png")), "&New game...", this, SLOT(slotNewSingleGame()), Qt::CTRL+Qt::Key_N);
  fileMenu->addSeparator();
  actFileOpen = fileMenu->addAction(QIcon(QString(":/pics/fileopen.png")), "&Open...", this, SLOT(slotFileOpen()), Qt::CTRL+Qt::Key_O);
  actFileSave = fileMenu->addAction(QIcon(QString(":/pics/filesave.png")), "&Save", this, SLOT(slotFileSave()), Qt::CTRL+Qt::Key_S);
  fileMenu->addSeparator();
  actOptions = fileMenu->addAction("&Options", this, SLOT(slotOptions()), Qt::CTRL+Qt::Key_P);
  fileMenu->addSeparator();
  actQuit = fileMenu->addAction("Quit", qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
  actFileSave->setEnabled(false);
  actOptions->setEnabled(true);

  viewMenu = menuBar()->addMenu("&Show");
  viewMenu->addAction(QIcon(QString(":/pics/paper.png")), "Score...", this, SLOT(slotShowScore()), Qt::CTRL+Qt::Key_R);

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
    mDesktop->loadGame(fileName);
    actFileSave->setEnabled(true);
    mDesktop->runGame();
  }
}


void Kpref::slotFileSave () {
  if (mDesktop) {
    QString fn = QFileDialog::getSaveFileName(this, "Select file to save the current game", "", "*.prf");
    if (!fn.isEmpty()) mDesktop->saveGame(fn);
  }
}


/*
void Kpref::slotFileQuit() {
  //quit();
}
*/


void Kpref::slotShowScore () {
  mDesktop->closePool();
  mDesktop->mShowPool = true;
  mDesktop->drawPool();
  mDesktop->mDeskView->mySleep(-1);
  mDesktop->mShowPool = false;
  mDesktop->draw();
}


void Kpref::forceRepaint () {
  update();
}


void Kpref::slotNewSingleGame () {
  if (mDesktop) {
    delete mDesktop;
    delete mDeskView;
    mDeskView = new DeskView(width(), height());
    mDesktop = new PrefDesktop();
    mDesktop->mDeskView = mDeskView;
    connect(mDesktop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
    connect(mDeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
  }
  actOptions->setEnabled(false);
  actFileOpen->setEnabled(false);
  mDesktop->runGame();
  actFileOpen->setEnabled(true);
  actOptions->setEnabled(true);
}


void Kpref::mouseMoveEvent (QMouseEvent *event) {
  if (mInMouseMoveEvent) return;
  mInMouseMoveEvent = true;
  Player *plr = mDesktop->currentPlayer();
  if (plr) plr->hilightCard(event->x(), event->y());
  mInMouseMoveEvent = false;
}


void Kpref::mousePressEvent (QMouseEvent *event) {
  mWaitingMouseUp = true;
  if (event->button() == Qt::LeftButton) {
    mDeskView->Event = 1;
    Player *plr = mDesktop->currentPlayer();
    if (plr) {
      plr->mClickX = event->x();
      plr->mClickY = event->y();
    }
  }
  mWaitingMouseUp = false;
}


void  Kpref::keyPressEvent (QKeyEvent *event) {
  if (mDeskView) {
    switch (event->key()) {
      case Qt::Key_Escape:
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Space:
        mDeskView->Event = 2;
        break;
      default: ;
    }
  }
}


void Kpref::paintEvent (QPaintEvent *event) {
  Q_UNUSED(event)
  if (mInPaintEvent) return;
  mInPaintEvent = true;
  if (mDeskView) {
    mDeskView->DesktopHeight = height();
    mDeskView->DesktopWidth = width();
  }
  if (mDeskView && mDeskView->mDeskBmp) {
    QPainter p;
    p.begin(this);
    //p.drawPixmap(0, 0, *(mDeskView->mDeskBmp));
    p.drawImage(0, 0, *(mDeskView->mDeskBmp));
    p.end();
  }
  mInPaintEvent = false;
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


void Kpref::saveOptions () {
  QSettings st;
  st.setValue("maxpool", optMaxPool);
  st.setValue("stalin", optStalingrad);
  st.setValue("whist10", opt10Whist);
  st.setValue("whistgreedy", optWhistGreedy);
}


void Kpref::loadOptions () {
  QSettings st;
  optMaxPool = st.value("maxpool", 10).toInt();
  if (optMaxPool < 4) optMaxPool = 4; else if (optMaxPool > 1000) optMaxPool = 1000;
  optStalingrad = st.value("stalin", true).toBool();
  opt10Whist = st.value("whist10", false).toBool();
  optWhistGreedy = st.value("whistgreedy", true).toBool();
}


void Kpref::slotOptions () {
  OptDialog *dlg = new OptDialog;
  dlg->sbGame->setValue(optMaxPool);
  dlg->cbGreedy->setChecked(optWhistGreedy);
  dlg->cbTenCheck->setChecked(!opt10Whist);
  dlg->cbStalin->setChecked(optStalingrad);
  if (dlg->exec() == QDialog::Accepted) {
    optMaxPool = dlg->sbGame->value();
    optWhistGreedy = dlg->cbGreedy->isChecked();
    opt10Whist = !dlg->cbTenCheck->isChecked();
    optStalingrad = dlg->cbStalin->isChecked();
    saveOptions();
  }
  delete dlg;
}
