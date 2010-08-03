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

  loadOptions();

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
  setMinimumWidth(CARDWIDTH*14.42);
  if (CARDHEIGHT*6 > 570)
  	setMinimumHeight(CARDHEIGHT*6);
  else
  	setMinimumHeight(570);
  //setFixedSize(w, h);
  resize(w, h);

  mDeskView = new DeskView(this);
  mDeskView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setCentralWidget(mDeskView);
  mDeskView->setAutoFillBackground(false);
  m_PrefModel = new PrefModel(mDeskView);
  mDeskView->setModel(m_PrefModel);
  connect(m_PrefModel, SIGNAL(deskChanged()), mDeskView, SLOT(update()));
  connect(m_PrefModel, SIGNAL(showHint(QString)), this, SLOT(showHint(QString)));
  connect(m_PrefModel, SIGNAL(clearHint()), this, SLOT(clearHint()));
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


void MainWindow::adjustDesk () {
  if (mDeskView) mDeskView->ClearScreen();
  mDeskView->update();
}


void MainWindow::initMenuBar () {
  QMenu *fileMenu = menuBar()->addMenu(tr("&Game"));

  actNewGame = fileMenu->addAction(QIcon(QString(":/pics/newgame.png")), tr("&New game..."), this, SLOT(slotNewSingleGame()), Qt::CTRL+Qt::Key_N);

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


/*
void Kpref::slotFileQuit() {
  //quit();
}
*/


void MainWindow::slotShowScore () {
  m_PrefModel->closePool();
  mDeskView->drawPool();
}


void MainWindow::slotNewSingleGame () {

  NewGameDialog *dlg = new NewGameDialog(this);
  connect(dlg->cbRounds, SIGNAL(stateChanged(int)), dlg, SLOT(toggleRounds(int)));
  // Players
  dlg->leHumanName->setText(optHumanName);
  dlg->leName1->setText(optPlayerName1);
  dlg->cbAlphaBeta1->setChecked(optAlphaBeta1);
  dlg->leName2->setText(optPlayerName2);
  dlg->cbAlphaBeta2->setChecked(optAlphaBeta2);
  // Conventions
  dlg->sbGame->setValue(optMaxPool);
  if(optQuitAfterMaxRounds) {
    dlg->cbRounds->setCheckState(Qt::Checked);
    dlg->sbRounds->setValue(optMaxRounds);
  }
  dlg->cbGreedy->setChecked(optWhistGreedy);
  dlg->rbTenWhist->setChecked(opt10Whist);
  dlg->cbStalin->setChecked(optStalingrad);
  dlg->cbAggPass->setChecked(optAggPass);
  dlg->cbWithoutThree->setChecked(optWithoutThree);
  
  if (dlg->exec() == QDialog::Accepted) {
	// Players
	optHumanName = dlg->leHumanName->text();
	optPlayerName1 = dlg->leName1->text();
    optAlphaBeta1 = dlg->cbAlphaBeta1->isChecked();
	optPlayerName2 = dlg->leName2->text();
	optAlphaBeta2 = dlg->cbAlphaBeta2->isChecked();
    // Conventions
  optMaxPool = dlg->sbGame->value();
  optQuitAfterMaxRounds = (dlg->cbRounds->checkState() == Qt::Checked);
  if (optQuitAfterMaxRounds) {
    optMaxRounds = dlg->sbRounds->value();
  }
    optWhistGreedy = dlg->cbGreedy->isChecked();
    opt10Whist = dlg->rbTenWhist->isChecked();
    optStalingrad = dlg->cbStalin->isChecked();
    optAggPass = dlg->cbAggPass->isChecked();
	optWithoutThree = dlg->cbWithoutThree->isChecked();
    saveOptions();
	delete dlg;
  }
  else
  {
	delete dlg;
	return;
  }
  
  mDeskView->ClearScreen();
  delete m_PrefModel;
  m_PrefModel = new PrefModel(mDeskView);    
  mDeskView->setModel(m_PrefModel);
  connect(m_PrefModel, SIGNAL(deskChanged()), mDeskView, SLOT(update()));
  connect(m_PrefModel, SIGNAL(showHint(QString)), this, SLOT(showHint(QString)));
  connect(m_PrefModel, SIGNAL(clearHint()), this, SLOT(clearHint()));
  
  optPassCount = 0;
  actFileOpen->setEnabled(false);
  actFileSave->setEnabled(true); 
  m_PrefModel->runGame();
  actFileOpen->setEnabled(true);
  actFileSave->setEnabled(false);
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
  st.setValue("maxpool", optMaxPool);
  st.setValue("quitmaxrounds", optQuitAfterMaxRounds);
  st.setValue("maxrounds", optMaxRounds);
  st.setValue("stalin", optStalingrad);
  st.setValue("whist10", opt10Whist);
  st.setValue("whistgreedy", optWhistGreedy);
  st.setValue("animdeal", optDealAnim);
  st.setValue("animtake", optTakeAnim);
  st.setValue("humanname", optHumanName);
  st.setValue("playername1", optPlayerName1);
  st.setValue("alphabeta1", optAlphaBeta1);
  st.setValue("playername2", optPlayerName2);
  st.setValue("alphabeta2", optAlphaBeta2);
  st.setValue("debughand", optDebugHands);
  st.setValue("without3", optWithoutThree);
  st.setValue("aggpass", optAggPass);
  st.setValue("prefclub", optPrefClub);
}


void MainWindow::loadOptions () {
  QSettings st;
  optMaxPool = st.value("maxpool", 10).toInt();
  if (optMaxPool < 4) optMaxPool = 4; else if (optMaxPool > 1000) optMaxPool = 1000;
  optStalingrad = st.value("stalin", false).toBool();
  opt10Whist = st.value("whist10", false).toBool();// true => radio button checks 'check' nevertheless!
  optWhistGreedy = st.value("whistgreedy", true).toBool();
  optDealAnim = st.value("animdeal", true).toBool();
  optTakeAnim = st.value("animtake", true).toBool();
  #ifndef WIN32	// May be #ifdef POSIX?
  	optHumanName = st.value("humanname", getenv("USER")).toString();
  #else
  	optHumanName = st.value("humanname", "").toString();
  #endif
  optPlayerName1 = st.value("playername1", tr("Player 1")).toString();
  optAlphaBeta1 = (st.value("alphabeta1", false).toBool());
  optPlayerName2 = st.value("playername2", tr("Player 2")).toString();
  optAlphaBeta2 = (st.value("alphabeta2", false).toBool());
  optWithoutThree = st.value("without3", false).toBool();
  optDebugHands = st.value("debughand", false).toBool();
  optAggPass = st.value("aggpass", false).toBool();
  optPrefClub = st.value("prefclub", false).toBool();
}


void MainWindow::slotOptions () {
  bool oldPrefClub = optPrefClub;
  bool oldDebugHands = optDebugHands;
  
  OptDialog *dlg = new OptDialog(this);
  //connect(dlg->cbPrefClub, SIGNAL(clicked()), this, SLOT(slotDeckChanged()));
  dlg->cbAnimDeal->setChecked(optDealAnim);
  dlg->cbAnimTake->setChecked(optTakeAnim);
  dlg->cbDebugHands->setChecked(optDebugHands);
  dlg->cbPrefClub->setChecked(optPrefClub);
  
  if (dlg->exec() == QDialog::Accepted) {
    optDealAnim = dlg->cbAnimDeal->isChecked();
    optTakeAnim = dlg->cbAnimTake->isChecked();
	optPrefClub = dlg->cbPrefClub->isChecked();
    optDebugHands = dlg->cbDebugHands->isChecked();
    saveOptions();
  }
  delete dlg;

  if ((optPrefClub != oldPrefClub) || (optDebugHands != oldDebugHands))
  	slotDeckChanged();
}

void MainWindow::slotDeckChanged () {
	mDeskView->freeCards();
	mDeskView->loadCards();
	setMinimumWidth(CARDWIDTH*14.42);
  	if (CARDHEIGHT*6 > 570)
  		setMinimumHeight(CARDHEIGHT*6);
  	else
  		setMinimumHeight(570);
	m_PrefModel->draw();
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

/*void Kpref::slotAbortBid () {
	if ((formBid->_GamesType != showpool) && (formBid->_GamesType != zerogame))
	{
		slotAbort();
		formBid->_GamesType = zerogame;
	}
}*/

void MainWindow::MoveImpossible () {
	HintBar->showMessage(tr("This move is impossible"));
}

void MainWindow::HintMove () {
	if (m_PrefModel->mBiddingDone)
		HintBar->showMessage(tr("Your move"));
	else
		HintBar->showMessage(tr("Select two cards to drop"));
}

void MainWindow::showHint(QString hint)
{
  HintBar->showMessage(hint);
}

void MainWindow::clearHint()
{
  HintBar->clearMessage();
}

bool MainWindow::WhistType () {
	int ret = QMessageBox::question(kpref, tr("Choose whist type"),
        tr("Do you want to whist with opened cards?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No | QMessageBox::Escape);
	if (ret == QMessageBox::Yes)
		return false;
	else
		return true;
}

const char * GenName(QString str, QString ext)
{
  int dot_pos=str.indexOf(ext);
  if (dot_pos == -1)
    str += ext;
  return str.toLocal8Bit();
}
