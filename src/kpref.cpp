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

#include "kpref.h"
#include "desktop.h"
#include "deskview.h"

#include <QDesktopWidget>
#include <QSettings>

//#include <QTextBrowser>

#include <QDebug>

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QStatusBar>

#include "formbid.h"
#include "newgameform.h"
#include "optform.h"
#include "helpbrowser.h"

inline static const char * GenName(QString str, QString ext);

//char *documentation; //see bottom this file
MainWindow *kpref;

MainWindow::MainWindow () {
  setWindowTitle("OpenPref");
  
  initMenuBar();
  mDeskView = 0;
  m_PrefModel = 0;

  HintBar = new QStatusBar(this);
  HintBar->setSizeGripEnabled(true);
  setStatusBar(HintBar);
  //HintBar->setFixedHeight(HINTBAR_MAX_HEIGHT);
  
  QDesktopWidget *desk = QApplication::desktop();
  QRect dims(desk->availableGeometry(this));
  int w = dims.width()-60;
  //w = 528*2;
  int h = dims.height()-120;
  //h = 800;
  int x = dims.left()+(dims.width()-w)/2;
  int y = dims.top()+(dims.height()-h)/2;
  move(x, y);

  mDeskView = new DeskView(this);
  mDeskView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setCentralWidget(mDeskView);
  resize(w, h);
  m_PrefModel = new PrefModel(mDeskView);

  // Load conventions
//  QSettings st;
//  m_PrefModel->optMaxPool = st.value("maxpool", 10).toInt();
  
  mDeskView->setModel(m_PrefModel);
  loadOptions();
  doConnects();
  HintBar->showMessage(tr("Welcome to OpenPref!"));
  FormBid *formBid = FormBid::instance(mDeskView);
  formBid->hide();
}


MainWindow::~MainWindow () {
  //delete StatusBar1;
  //!delete formBid;
  mDeskView->deleteLater();
  m_PrefModel->deleteLater();
  HintBar->deleteLater();
  //delete Hint;
}

inline void MainWindow::doConnects()
{
  connect(m_PrefModel, SIGNAL(deskChanged()), mDeskView, SLOT(update()));
  connect(m_PrefModel, SIGNAL(showHint(QString)), this, SLOT(showHint(QString)));
  connect(m_PrefModel, SIGNAL(clearHint()), this, SLOT(clearHint()));
}


void MainWindow::adjustDesk () {
  if (mDeskView) mDeskView->ClearScreen();
  mDeskView->update();
}


void MainWindow::initMenuBar () {
  QMenu *fileMenu = menuBar()->addMenu(tr("&Game"));

  fileMenu->addAction(QIcon(QString(":/pics/newgame.png")), tr("&New game..."), this, SLOT(slotNewSingleGame()), Qt::CTRL+Qt::Key_N);

  fileMenu->addSeparator();

  actFileOpen = fileMenu->addAction(QIcon(QString(":/pics/fileopen.png")), tr("&Open..."), this, SLOT(slotFileOpen()), Qt::CTRL+Qt::Key_O);
  actFileSave = fileMenu->addAction(QIcon(QString(":/pics/filesave.png")), tr("&Save"), this, SLOT(slotFileSave()), Qt::CTRL+Qt::Key_S);

  fileMenu->addSeparator();

  QAction *actOptions = fileMenu->addAction(QIcon(QString(":/pics/tool.png")), tr("&Options..."), this, SLOT(slotOptions()), Qt::CTRL+Qt::Key_P);
  fileMenu->addSeparator();
  //actQuit = fileMenu->addAction(QIcon(QString(":/pics/exit.png")), tr("&Quit"), qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
  fileMenu->addAction(QIcon(QString(":/pics/exit.png")), tr("&Quit"), this, SLOT(slotQuit()), Qt::CTRL+Qt::Key_Q);
  
  actFileSave->setEnabled(false);
  actOptions->setEnabled(true);

  QMenu *viewMenu = menuBar()->addMenu(tr("&Show"));
  viewMenu->addAction(QIcon(QString(":/pics/paper.png")), tr("S&core..."), this, SLOT(slotShowScore()), Qt::CTRL+Qt::Key_R);

  menuBar()->addSeparator();

  QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(tr("&Preferans Rules..."), this, SLOT(slotRules()), Qt::Key_F1);
  helpMenu->addAction(tr("&About OpenPref"), this, SLOT(slotHelpAbout()), 0);
  helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
}


///////////////////////////////////////////////////////////////////////////////
// slots
///////////////////////////////////////////////////////////////////////////////
void MainWindow::slotFileOpen () {
  QString fileName = QFileDialog::getOpenFileName(this, "Select saved game", "", "*.prf");
  if (!fileName.isEmpty())  {
    m_PrefModel->loadGame(fileName);
    actFileSave->setEnabled(true);
    m_PrefModel->runGame();
  }
}


void MainWindow::slotFileSave () {
  if (m_PrefModel) {
    QString fn = QFileDialog::getSaveFileName(this, "Select file to save the current game", "", "*.prf");
    if (!fn.isEmpty()) {
		fn = GenName(fn, ".prf");
		m_PrefModel->saveGame(fn);
	}
  }
}


void MainWindow::slotShowScore () {
  m_PrefModel->closePool();
  mDeskView->drawPool();
}


void MainWindow::slotNewSingleGame ()
{
  QSettings st;
  NewGameDialog *dlg = new NewGameDialog(this);
  connect(dlg->cbRounds, SIGNAL(stateChanged(int)), dlg, SLOT(toggleRounds(int)));
  // Players
  dlg->leHumanName->setText(m_PrefModel->optHumanName);
  dlg->leName1->setText(st.value("playername1", tr("Player 1")).toString());
  dlg->leName2->setText(st.value("playername2", tr("Player 2")).toString());
  dlg->cbAlphaBeta1->setChecked(st.value("alphabeta1", false).toBool());
  dlg->cbAlphaBeta2->setChecked(st.value("alphabeta2", false).toBool());
  // Conventions
  //dlg->sbGame->setValue(m_PrefModel->optMaxPool);

  dlg->sbGame->setValue(st.value("maxpool", 10).toInt());
  /*if(optQuitAfterMaxRounds) {
    dlg->cbRounds->setCheckState(Qt::Checked);
    dlg->sbRounds->setValue(optMaxRounds);
  }*/
  if(st.value("quitmaxrounds", false).toBool()) {
    dlg->cbRounds->setCheckState(Qt::Checked);
    dlg->sbRounds->setValue(st.value("maxrounds", -1).toInt());
  }
  dlg->cbGreedy->setChecked(st.value("whistgreedy", true).toBool());
  dlg->rbTenWhist->setChecked(st.value("whist10", false).toBool());
  dlg->cbStalin->setChecked(st.value("stalin", false).toBool());
  dlg->cbAggPass->setChecked(st.value("aggpass", false).toBool());
  dlg->cbWithoutThree->setChecked(st.value("without3", false).toBool());
  
  if (dlg->exec() == QDialog::Accepted) {
    mDeskView->ClearScreen();
    delete m_PrefModel;
    m_PrefModel = new PrefModel(mDeskView);
    mDeskView->setModel(m_PrefModel);
    doConnects();
    
    // Conventions
  //st.setValue("maxpool",dlg->sbGame->value());
    m_PrefModel->optMaxPool = dlg->sbGame->value();
    m_PrefModel->optQuitAfterMaxRounds = (dlg->cbRounds->checkState() == Qt::Checked);
    if (m_PrefModel->optQuitAfterMaxRounds) {
      m_PrefModel->optMaxRounds = dlg->sbRounds->value();
    }
    m_PrefModel->optWhistGreedy = dlg->cbGreedy->isChecked();
    m_PrefModel->opt10Whist = dlg->rbTenWhist->isChecked();
    m_PrefModel->optStalingrad = dlg->cbStalin->isChecked();
    m_PrefModel->optAggPass = dlg->cbAggPass->isChecked();
    m_PrefModel->optWithoutThree = dlg->cbWithoutThree->isChecked();
    m_PrefModel->optPassCount = 0;

    // Players
    m_PrefModel->optHumanName = dlg->leHumanName->text();
    m_PrefModel->optPlayerName1 = dlg->leName1->text();
    m_PrefModel->optAlphaBeta1 = dlg->cbAlphaBeta1->isChecked();
    m_PrefModel->optPlayerName2 = dlg->leName2->text();
    m_PrefModel->optAlphaBeta2 = dlg->cbAlphaBeta2->isChecked();
  
    saveOptions();
    //actFileOpen->setEnabled(false);
    actFileSave->setEnabled(true); 
    m_PrefModel->runGame();
    //actFileOpen->setEnabled(true);
    actFileSave->setEnabled(false);
  }
  delete dlg;

    // Load conventions
  //m_PrefModel->optMaxPool = st.value("maxpool", 10).toInt();
}


void  MainWindow::keyPressEvent (QKeyEvent *event) {
  qApp->notify(mDeskView, event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
	 int ret;
	if (!m_PrefModel->mGameRunning)
		exit(0);	 
	 ret = QMessageBox::question(this, tr("OpenPref"),
        tr("Do you really want to quit the game?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No | QMessageBox::Escape);
	if (ret == QMessageBox::Yes)
         exit(0);
    else
         event->ignore();
 }

void MainWindow::slotHelpAbout () {
  QMessageBox::about(this, tr("About"),
    tr("<h2 align=center>&spades; <font color=red>&diams;</font> OpenPref " OPENPREF_VERSION " &clubs; <font color=red>&hearts;</font></h2>"
  "<p align=center>Open source cross-platform Preferans game</p>"
  "<p align=center><a href=\"http://openpref.sourceforge.net/\">http://openpref.sourceforge.net</a></p>\n"
  "<p align=center>Copyright &copy;2000-2010, OpenPref Developers:<br/>I.Azarniy<br/>A.V.Fedotov<br/>Ketmar Dark<br/>K.Tokarev</p>"
	"<p>OpenPref is free software; you can redistribute it and/or modify<br/>"
    "it under the terms of the GNU General Public License (see file<br/>"
    "COPYING or <a href=\"http://www.gnu.org/licenses\">http://www.gnu.org/licenses</a>)</p>")
  );
}


void MainWindow::saveOptions () {
  QSettings st;
  // Conventions
  st.setValue("maxpool", m_PrefModel->optMaxPool);
  st.setValue("quitmaxrounds", m_PrefModel->optQuitAfterMaxRounds);
  st.setValue("maxrounds", m_PrefModel->optMaxRounds);
  st.setValue("stalin", m_PrefModel->optStalingrad);
  st.setValue("whist10", m_PrefModel->opt10Whist);
  st.setValue("whistgreedy", m_PrefModel->optWhistGreedy);
  st.setValue("without3", m_PrefModel->optWithoutThree);
  st.setValue("aggpass", m_PrefModel->optAggPass);

  st.setValue("humanname", m_PrefModel->optHumanName);
  st.setValue("playername1", m_PrefModel->optPlayerName1);
  st.setValue("alphabeta1", m_PrefModel->optAlphaBeta1);
  st.setValue("playername2", m_PrefModel->optPlayerName2);
  st.setValue("alphabeta2", m_PrefModel->optAlphaBeta2);

  // View settings
  st.setValue("prefclub", mDeskView->optPrefClub);
  st.setValue("animdeal", mDeskView->optDealAnim);
  st.setValue("animtake", mDeskView->optTakeAnim);
  st.setValue("debughand", mDeskView->optDebugHands);
}


void MainWindow::loadOptions () {
  QSettings st;
  //m_PrefModel->optMaxPool = st.value("maxpool", 10).toInt();
  //if (m_PrefModel->optMaxPool < 4)
    //m_PrefModel->optMaxPool = 4;
  //else if (m_PrefModel->optMaxPool > 1000)
    //m_PrefModel->optMaxPool = 1000;
  //optStalingrad = st.value("stalin", false).toBool();
  //opt10Whist = st.value("whist10", false).toBool();// true => radio button checks 'check' nevertheless!
  //optWhistGreedy = st.value("whistgreedy", true).toBool();
  mDeskView->optDealAnim = st.value("animdeal", true).toBool();
  mDeskView->optTakeAnim = st.value("animtake", true).toBool();
  #ifndef WIN32	// May be #ifdef POSIX?
  	m_PrefModel->optHumanName = st.value("humanname", getenv("USER")).toString();
  #else
  	m_PrefModel->optHumanName = st.value("humanname", "").toString();
  #endif
  m_PrefModel->optPlayerName1 = st.value("playername1", tr("Player 1")).toString();
  m_PrefModel->optAlphaBeta1 = (st.value("alphabeta1", false).toBool());
  m_PrefModel->optPlayerName2 = st.value("playername2", tr("Player 2")).toString();
  m_PrefModel->optAlphaBeta2 = (st.value("alphabeta2", false).toBool());
  //optWithoutThree = st.value("without3", false).toBool();
  mDeskView->optDebugHands = st.value("debughand", false).toBool();
  //optAggPass = st.value("aggpass", false).toBool();
  mDeskView->optPrefClub = st.value("prefclub", false).toBool();
}


void MainWindow::slotOptions () {
  bool oldPrefClub = mDeskView->optPrefClub;
  bool oldDebugHands = mDeskView->optDebugHands;
  
  OptDialog *dlg = new OptDialog(this);
  //connect(dlg->cbPrefClub, SIGNAL(clicked()), this, SLOT(slotDeckChanged()));
  dlg->cbAnimDeal->setChecked(mDeskView->optDealAnim);
  dlg->cbAnimTake->setChecked(mDeskView->optTakeAnim);
  dlg->cbDebugHands->setChecked(mDeskView->optDebugHands);
  dlg->cbPrefClub->setChecked(mDeskView->optPrefClub);
  
  if (dlg->exec() == QDialog::Accepted) {
    mDeskView->optDealAnim = dlg->cbAnimDeal->isChecked();
    mDeskView->optTakeAnim = dlg->cbAnimTake->isChecked();
    mDeskView->optPrefClub = dlg->cbPrefClub->isChecked();
    mDeskView->optDebugHands = dlg->cbDebugHands->isChecked();
    saveOptions();
  }
  delete dlg;

  if ((mDeskView->optPrefClub != oldPrefClub) || (mDeskView->optDebugHands != oldDebugHands))
    slotDeckChanged();
}

void MainWindow::slotDeckChanged () {
	mDeskView->reloadCards();
	mDeskView->draw();
}

void MainWindow::slotRules () {
	QMessageBox::about(this, tr("Preferans Rules"), tr("Help system has not been implemented yet!\nSee http://en.wikipedia.org/wiki/Preferans"));
  //HelpBrowser *dlg = new HelpBrowser;
  //dlg->tbHelp->setSearchPaths(QStringList("/home/kostya/projects/Qt/doc/html"));
  //dlg->tbHelp->loadResource(0, QUrl::fromLocalFile("/home/kostya/projects/Qt/doc/html/index.html"));
  // delete dlg;
}

void MainWindow::slotQuit () {
	int ret;
	if (!m_PrefModel->mGameRunning)
		exit(0);
	ret = QMessageBox::question(this, tr("OpenPref"),
        tr("Do you really want to quit the game?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No | QMessageBox::Escape);
	if (ret == QMessageBox::Yes)
  		exit(0);	
}

void MainWindow::showHint(QString hint)
{
  HintBar->showMessage(hint);
}

void MainWindow::clearHint()
{
  HintBar->clearMessage();
}

const char * GenName(QString str, QString ext)
{
  int dot_pos=str.indexOf(ext);
  if (dot_pos == -1)
    str += ext;
  return str.toLocal8Bit();
}
