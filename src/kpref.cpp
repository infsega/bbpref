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

#include <QSettings>
#include <QDebug>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QStatusBar>

#include <stdbool.h>
#include <sys/platform.h>
#include <bps/bps.h>
#include "bps/event.h"
#include <bps/locale.h>
#include <bps/navigator.h>
#include <bps/virtualkeyboard.h>

#include "formbid.h"
#include "newgameform.h"
#include "optiondialog.h"
#include "playersinfodialog.h"
#include "scorehistory.h"
#include "aboutdialog.h"

inline const char * GenName(const QString &str, const QString &ext);

MainWindow::MainWindow()
{
  setWindowTitle("BB Pref");
  setWindowIcon(QIcon(":/pics/newgame.png"));

  mDeskView = 0;
  m_PrefModel = 0;

  HintBar = new QStatusBar(this);
  HintBar->setSizeGripEnabled(true);
  setStatusBar(HintBar);

  mDeskView = new DeskView(this);
  setCentralWidget(mDeskView);
  m_PrefModel = new PrefModel(mDeskView);
  mDeskView->setModel(m_PrefModel);
  readSettings();
  doConnects();
  initMenuBar();
  HintBar->showMessage(tr("Welcome to BB Pref!"));
  BidDialog::instance(mDeskView)->hide();
}


MainWindow::~MainWindow()
{
  delete mDeskView;
  delete m_PrefModel;
}

inline void MainWindow::doConnects()
{
  connect(m_PrefModel, SIGNAL(deskChanged()), mDeskView, SLOT(update()));
  connect(m_PrefModel, SIGNAL(showHint(const QString&)), this, SLOT(showHint(const QString&)));
  connect(m_PrefModel, SIGNAL(clearHint()), this, SLOT(clearHint()));
  connect(m_PrefModel, SIGNAL(gameChanged(QString)), this, SLOT(changeTitle(QString)));
}


void MainWindow::adjustDesk()
{
  if (mDeskView)
    mDeskView->ClearScreen();
  mDeskView->update();
}


void MainWindow::initMenuBar()
{
  // Load icons
  const QIcon openicon = QIcon::fromTheme("document-open", QIcon(":/pics/fileopen.png"));
  const QIcon saveicon = QIcon::fromTheme("document-save", QIcon(":/pics/filesave.png"));
  const QIcon toolicon = QIcon::fromTheme("preferences-system", QIcon(":/pics/tool.png"));
  const QIcon exiticon = QIcon::fromTheme("application-exit", QIcon(":/pics/exit.png"));
  const QIcon helpicon = QIcon::fromTheme("system-help", QIcon(":/pics/help.png"));
  QMenu *fileMenu = menuBar()->addMenu(tr("&Game"));

  fileMenu->addAction(QIcon(":/pics/newgame.png"), tr("&New game..."),
                      this, SLOT(newSingleGame()), QKeySequence::New);

  fileMenu->addSeparator();

  actFileOpen = fileMenu->addAction(openicon, tr("&Open..."), this, SLOT(openFile()), QKeySequence::Open);
  actFileSave = fileMenu->addAction(saveicon, tr("&Save"), this, SLOT(saveFile()), QKeySequence::Save);

  fileMenu->addSeparator();

  QAction *actOptions = fileMenu->addAction(toolicon, tr("&Options..."), this, SLOT(showOptions()), QKeySequence::Preferences);
  fileMenu->addSeparator();
  fileMenu->addAction(exiticon, tr("&Quit"), this, SLOT(quitGame()), QKeySequence::Quit);
  actFileSave->setEnabled(false);
  actOptions->setEnabled(true);

  QMenu *viewMenu = menuBar()->addMenu(tr("&Show"));
  viewMenu->addAction(QIcon(":/pics/paper.png"), tr("S&core..."), this, SLOT(showScore()), Qt::CTRL+Qt::Key_R);
  viewMenu->addAction(QIcon(":/pics/unknown-player.png"), tr("Players..."), this, SLOT(showPlayers()));
  viewMenu->addAction(tr("Game Log..."), this, SLOT(showLog()));

  menuBar()->addSeparator();

  QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(helpicon, tr("&Preferans Rules..."), this, SLOT(helpRules()), QKeySequence::HelpContents);
  helpMenu->addSeparator();
  helpMenu->addAction(QIcon(":/pics/newgame.png"), tr("&About..."), this, SLOT(helpAbout()), 0);
}


///////////////////////////////////////////////////////////////////////////////
// slots
///////////////////////////////////////////////////////////////////////////////
void MainWindow::openFile()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Select saved game", "", "*.prf");
  if (!fileName.isEmpty())
  {
    m_PrefModel->loadGame(fileName);
    actFileSave->setEnabled(true);
    m_PrefModel->runGame();
  }
}


void MainWindow::saveFile()
{
  if (m_PrefModel)
  {
    QString fn = QFileDialog::getSaveFileName(this, "Select file to save the current game", "", "*.prf");
    if (!fn.isEmpty())
    {
      fn = GenName(fn, ".prf");
      m_PrefModel->saveGame(fn);
	}
  }
}


void MainWindow::showScore()
{
  m_PrefModel->closePool();
  mDeskView->drawPool();
}

void MainWindow::showPlayers()
{
  PlayersInfoDialog dlg(m_PrefModel, this);
  dlg.exec();
}

void MainWindow::showLog()
{
  ScoreHistoryDialog dlg(m_PrefModel, this);
  dlg.exec();
}

void MainWindow::newSingleGame()
{
  NewGameDialog dlg(m_PrefModel, this);
  if (dlg.exec() == QDialog::Accepted)
  {
    virtualkeyboard_hide();
    mDeskView->ClearScreen();
    delete m_PrefModel;
    m_PrefModel = new PrefModel(mDeskView);
    mDeskView->setModel(m_PrefModel);
    doConnects();
    dlg.copyOptions(*m_PrefModel);
    writeSettings();
    actFileSave->setEnabled(true); 
    m_PrefModel->runGame();
    actFileSave->setEnabled(false);
  }
}

void  MainWindow::keyPressEvent (QKeyEvent *event)
{
  qApp->notify(mDeskView, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (quitGame())
    exit(0);
  else
    event->ignore();
 }

void MainWindow::helpAbout()
{
  AboutDialog dlg(this);
  dlg.exec();
}

void MainWindow::writeSettings()
{
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
}

void MainWindow::readSettings()
{
  QSettings st;
  m_PrefModel->optHumanName   = st.value("humanname", getenv("USER")).toString();
  m_PrefModel->optPlayerName1 = st.value("playername1", tr("Player 1")).toString();
  m_PrefModel->optAlphaBeta1  = (st.value("alphabeta1", false).toBool());
  m_PrefModel->optPlayerName2 = st.value("playername2", tr("Player 2")).toString();
  m_PrefModel->optAlphaBeta2  = (st.value("alphabeta2", false).toBool());
}


void MainWindow::showOptions()
{
  OptionDialog opt(this);
  opt.setBackgroundType(mDeskView->backgroundType());
  opt.setBackgroundColor(mDeskView->backgroundColor());
  opt.cbAnimDeal->setChecked(mDeskView->optDealAnim);
  opt.cbAnimTake->setChecked(mDeskView->optTakeAnim);
  opt.hsTakeQuality->setEnabled(mDeskView->optTakeAnim);
  opt.hsTakeQuality->setValue(mDeskView->takeAnimQuality());
  opt.cbDebugHands->setChecked(mDeskView->optDebugHands);
  if (opt.exec() == QDialog::Accepted)
  {
    mDeskView->setBackgroundType(opt.backgroundType());
    mDeskView->setBackgroundColor(opt.backgroundColor());
    mDeskView->optDealAnim = opt.cbAnimDeal->isChecked();
    mDeskView->optTakeAnim = opt.cbAnimTake->isChecked();
    mDeskView->setTakeQuality(opt.hsTakeQuality->value());
    mDeskView->optDebugHands = opt.cbDebugHands->isChecked();

    if (opt.optionsModified())
      mDeskView->reloadCards();
  }
  mDeskView->draw(true);
  this->update();
}

void MainWindow::helpRules()
{
  char* country = NULL;
  char* language = NULL;
  locale_get(&language, &country);
  if ( strcmp(language, "ru") == 0 )
    navigator_invoke("http://ru.wikipedia.org/wiki/%D0%BF%D1%80%D0%B5%D1%84%D0%B5%D1%80%D0%B0%D0%BD%D1%81", NULL);
  else
    navigator_invoke("http://en.wikipedia.org/wiki/Preferans", NULL);
}

bool MainWindow::quitGame () {
	int ret;
	if (!m_PrefModel->mGameRunning) {
		mDeskView->writeSettings();
		exit(0);
		return true;
	}
    ret = QMessageBox::question(this, tr("BB Pref"),
        tr("Do you really want to quit the game?"),
        QMessageBox::Yes | QMessageBox::Default,
        QMessageBox::No | QMessageBox::Escape);
    if (ret == QMessageBox::Yes) {
        mDeskView->writeSettings();
        exit(0);
        return true;
    }
    return false;
}

void MainWindow::showHint(const QString & hint)
{
  HintBar->showMessage(hint);
}

void MainWindow::clearHint()
{
  HintBar->clearMessage();
}

void MainWindow::changeTitle(const QString & gameName)
{
  if (gameName.isEmpty())
    setWindowTitle("BB Pref");
  else
    setWindowTitle(QString("BB Pref: ") + gameName);
}

const char * GenName(const QString &str, const QString &ext)
{
  int dot_pos=str.indexOf(ext);
  if (dot_pos == -1)
    return (str + ext).toLocal8Bit();
  else
    return str.toLocal8Bit();
}
