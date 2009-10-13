/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
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

#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QMainWindow>
#include <QSettings>

#include <QTextBrowser>

#include "kpref.h"
#include <QDebug>

#include "desktop.h"
#include "deskview.h"
#include "formbid.h"
#include "newgameform.h"
#include "optform.h"
#include "helpbrowser.h"
#include "player.h"


#include <stdlib.h>
#include <string>
using std::string;

#define HINTBAR_MAX_HEIGHT 22

const char * GenName(string str, string ext);

//char *documentation; //see bottom this file
Kpref *kpref;


Kpref::Kpref () {
  setWindowTitle("OpenPref");
  
  initMenuBar();
  mInPaintEvent = mInMouseMoveEvent = mWaitingMouseUp = false;
  mDeskView = 0;
  mDesktop = 0;
  setMouseTracking(true);

  loadOptions();

  HintBar = new QStatusBar;
  HintBar->setSizeGripEnabled(true);
  setStatusBar(HintBar);
  HintBar->setFixedHeight(HINTBAR_MAX_HEIGHT);
  
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

 //connect(this, SIGNAL(rejected()), qApp, SLOT(quit()));
}


void Kpref::init () {
  //StatusBar1 = new QStatusBar(this);
  mDeskView = new DeskView(width(), height()-HINTBAR_MAX_HEIGHT);
  mDesktop = new PrefDesktop(mDeskView);
  connect(mDesktop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
  connect(mDeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
  HintBar->showMessage(tr("Welcome to OpenPref!"));
  
 // resize(width(),height()+HintBar->height());
}

void Kpref::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event)
  mDeskView->DesktopWidth=width();
  mDeskView->DesktopHeight=height()-HINTBAR_MAX_HEIGHT;
  mDesktop->draw();
  forceRepaint();
}


Kpref::~Kpref () {
  //delete StatusBar1;
  //!delete formBid;
  delete mDeskView;
  delete mDesktop;
  delete HintBar;
  delete Hint;
}


void Kpref::adjustDesk () {
  if (mDeskView) mDeskView->ClearScreen();
  update();
}


void Kpref::initMenuBar () {
  fileMenu = menuBar()->addMenu(tr("&Game"));
  actNewGame = fileMenu->addAction(QIcon(QString(":/pics/newgame.png")), tr("&New game..."), this, SLOT(slotNewSingleGame()), Qt::CTRL+Qt::Key_N);
  fileMenu->addSeparator();
  actFileOpen = fileMenu->addAction(QIcon(QString(":/pics/fileopen.png")), tr("&Open..."), this, SLOT(slotFileOpen()), Qt::CTRL+Qt::Key_O);
  actFileSave = fileMenu->addAction(QIcon(QString(":/pics/filesave.png")), tr("&Save"), this, SLOT(slotFileSave()), Qt::CTRL+Qt::Key_S);
  fileMenu->addSeparator();
  actOptions = fileMenu->addAction(QIcon(QString(":/pics/tool.png")), tr("&Options..."), this, SLOT(slotOptions()), Qt::CTRL+Qt::Key_P);
  fileMenu->addSeparator();
  //actQuit = fileMenu->addAction(QIcon(QString(":/pics/exit.png")), tr("&Quit"), qApp, SLOT(quit()), Qt::CTRL+Qt::Key_Q);
  actQuit = fileMenu->addAction(QIcon(QString(":/pics/exit.png")), tr("&Quit"), this, SLOT(slotQuit()), Qt::CTRL+Qt::Key_Q);
  actFileSave->setEnabled(false);
  actOptions->setEnabled(true);

  viewMenu = menuBar()->addMenu(tr("&Show"));
  viewMenu->addAction(QIcon(QString(":/pics/paper.png")), tr("S&core..."), this, SLOT(slotShowScore()), Qt::CTRL+Qt::Key_R);

  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(tr("&Preferans Rules..."), this, SLOT(slotRules()), Qt::Key_F1);
  helpMenu->addAction(tr("&About OpenPref"), this, SLOT(slotHelpAbout()), 0);
  helpMenu->addAction(tr("About &Qt"), qApp, SLOT(aboutQt()));
  //actAboutQt = helpMenu->addAction(tr("About &Qt"));
  //connect(actAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
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
    if (!fn.isEmpty()) {
		fn = GenName(fn.toStdString(), "prf");
		mDesktop->saveGame(fn);
	}
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

  NewGameDialog *dlg = new NewGameDialog;
  // Players
  dlg->leHumanName->setText(optHumanName);
  dlg->leName1->setText(optPlayerName1);
  dlg->cbAlphaBeta1->setChecked(optAlphaBeta1);
  dlg->leName2->setText(optPlayerName2);
  dlg->cbAlphaBeta2->setChecked(optAlphaBeta2);
  // Conventions
  dlg->sbGame->setValue(optMaxPool);
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
  
  if (mDesktop) {
    delete mDesktop;
    delete mDeskView;
    mDeskView = new DeskView(width(), height()-HINTBAR_MAX_HEIGHT);
    mDesktop = new PrefDesktop();
    mDesktop->mDeskView = mDeskView;
    connect(mDesktop, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
    connect(mDeskView, SIGNAL(deskChanged()), this, SLOT(forceRepaint()));
  }  
  
  optPassCount = 0;
  actFileOpen->setEnabled(false);
  actFileSave->setEnabled(true); 
  mDesktop->runGame();
  actFileOpen->setEnabled(true);
  actFileSave->setEnabled(false);
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
    mDeskView->DesktopHeight = height()-HINTBAR_MAX_HEIGHT;
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

void Kpref::closeEvent(QCloseEvent *event) {
	 int ret;
	if (!mDesktop->mGameRunning)
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

void Kpref::slotHelpAbout () {
  QMessageBox::about(this, tr("About"),
    tr("<h2 align=center>&spades; <font color=red>&diams;</font> OpenPref 0.1.3 &clubs; <font color=red>&hearts;</font></h2>"
	"<p>More information on <br/><a href=\"http://sourceforge.net/projects/openpref\">http://sourceforge.net/projects/openpref</a><br/>"
    "Get latest sources on <br/><a href=\"http://gitorious.org/openprefqt4\">http://gitorious.org/openprefqt4</a><br/></p>\n"
    "  <br/>Copyright &copy;2000-2009, OpenPref Developers:<br/>I.Azarniy, A.V.Fedotov, Ketmar Dark, K.Tokarev<br/>"
	"<br/>"
	"OpenPref is free software; you can redistribute it and/or modify<br/>"
    "it under the terms of the GNU General Public License (see file<br/>"
    "COPYING or <a href=\"http://www.gnu.org/licenses\">http://www.gnu.org/licenses</a>)")
  );
}


void Kpref::saveOptions () {
  QSettings st;
  st.setValue("maxpool", optMaxPool);
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


void Kpref::loadOptions () {
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


void Kpref::slotOptions () {
  bool oldPrefClub = optPrefClub;
  bool oldDebugHands = optDebugHands;
  
  OptDialog *dlg = new OptDialog;
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

void Kpref::slotDeckChanged () {
	mDeskView->freeCards();
	mDeskView->loadCards();
	setMinimumWidth(CARDWIDTH*14.42);
  	if (CARDHEIGHT*6 > 570)
  		setMinimumHeight(CARDHEIGHT*6);
  	else
  		setMinimumHeight(570);
	mDesktop->draw();
	forceRepaint();
}

void Kpref::slotRules () {
	QMessageBox::about(this, tr("Preferans Rules"), tr("Help system has not been implemented yet!\nSee http://en.wikipedia.org/wiki/Preferans"));
  //HelpBrowser *dlg = new HelpBrowser;
  //dlg->tbHelp->setSearchPaths(QStringList("/home/kostya/projects/Qt/doc/html"));
  //dlg->tbHelp->loadResource(0, QUrl::fromLocalFile("/home/kostya/projects/Qt/doc/html/index.html"));
  // delete dlg;
}

void Kpref::slotQuit () {
	int ret;
	if (!mDesktop->mGameRunning)
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

void Kpref::MoveImpossible () {
	HintBar->showMessage(tr("This move is impossible"));
}

void Kpref::HintMove () {
	if (mDesktop->mBiddingDone)
		HintBar->showMessage(tr("Your move"));
	else
		HintBar->showMessage(tr("Select two cards to drop"));
}

bool Kpref::WhistType () {
	int ret = QMessageBox::question(kpref, tr("Choose whist type"),
        tr("Do you want to whist with opened cards?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No | QMessageBox::Escape);
	if (ret == QMessageBox::Yes)
		return false;
	else
		return true;
}

const char * GenName(string str, string ext)
{
	size_t dot_pos=str.rfind('.');
	if (dot_pos!=string::npos)
		str.replace(dot_pos+1,str.length()-dot_pos,ext);
	else
	{
		str += ".";
		str += ext;
	}
	return str.c_str();
}
