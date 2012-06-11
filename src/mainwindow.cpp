#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "aboutdialog.h"
#include "gamelogdialog.h"
#include "optiondialog.h"
#include "desktop.h"
#include "newgameform.h"
#include "savegamedialog.h"
#include "loadgamedialog.h"

#include <bps/navigator.h>
#include <bps/locale.h>
#include <bps/virtualkeyboard.h>

MainWindow::MainWindow(QWidget *parent):
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  mDeskView = ui->widget;
  m_PrefModel = new PrefModel(mDeskView);
  mDeskView->setModel(m_PrefModel);

  readSettings();
  doConnects();

  ui->statusbar->showMessage(tr("Welcome to BB Pref!"));
  BidDialog::instance(mDeskView)->hide();
}

MainWindow::~MainWindow()
{
  delete ui;
  //delete mDeskView;
  delete m_PrefModel;
}

void MainWindow::doConnects()
{
  connect(m_PrefModel, SIGNAL(deskChanged()), mDeskView, SLOT(update()));
  connect(m_PrefModel, SIGNAL(showHint(const QString&)), ui->statusbar, SLOT(showMessage(QString)));
  connect(m_PrefModel, SIGNAL(clearHint()), ui->statusbar, SLOT(clearMessage()));
  //connect(m_PrefModel, SIGNAL(gameChanged(QString)), this, SLOT(changeTitle(QString)));
}

void MainWindow::adjustDesk()
{
  if (mDeskView)
    mDeskView->ClearScreen();
  mDeskView->update();
}

void MainWindow::on_actionNewGame_triggered()
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
    ui->actionSaveGame->setEnabled(true);
    m_PrefModel->runGame();
  }
}

void MainWindow::on_actionOpenGame_triggered()
{
  LoadGameDialog dlg(this);
  if (dlg.exec() == QDialog::Accepted)
  {
    QString filename = dlg.selection();
    if (!filename.isEmpty())
    {
      m_PrefModel->loadGame(filename);
      ui->actionSaveGame->setEnabled(true);
      m_PrefModel->runGame();
    }
  }
}

void MainWindow::on_actionSaveGame_triggered()
{
  if (!m_PrefModel)
    return;
  SaveGameDialog dlg(this);
  if (dlg.exec() == QDialog::Accepted)
    if (!dlg.selection().isEmpty())
      m_PrefModel->saveGame(dlg.selection());
}

void MainWindow::on_actionShowScore_triggered()
{
  m_PrefModel->closePool();
  mDeskView->drawPool();
}

void MainWindow::on_actionGameLog_triggered()
{
  GameLogDialog dlg(m_PrefModel, this);
  dlg.exec();
}

void MainWindow::on_actionOptions_triggered()
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
  update();
}

void MainWindow::on_actionRules_triggered()
{
  char* country = NULL;
  char* language = NULL;
  locale_get(&language, &country);
  if ( strcmp(language, "ru") == 0 )
    navigator_invoke("http://ru.wikipedia.org/wiki/%D0%BF%D1%80%D0%B5%D1%84%D0%B5%D1%80%D0%B0%D0%BD%D1%81", NULL);
  else
    navigator_invoke("http://en.wikipedia.org/wiki/Preferans", NULL);
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
  st.setValue("playername2", m_PrefModel->optPlayerName2);
  st.setValue("alphabeta1", m_PrefModel->optAlphaBeta1);
  st.setValue("alphabeta2", m_PrefModel->optAlphaBeta2);
}

void MainWindow::readSettings()
{
  QSettings st;
  m_PrefModel->optHumanName   = st.value("humanname", getenv("USER")).toString();
  m_PrefModel->optPlayerName1 = st.value("playername1", tr("Player 1")).toString();
  m_PrefModel->optPlayerName2 = st.value("playername2", tr("Player 2")).toString();
  m_PrefModel->optAlphaBeta1  = st.value("alphabeta1", false).toBool();
  m_PrefModel->optAlphaBeta2  = st.value("alphabeta2", false).toBool();
}

void MainWindow::on_actionAbout_triggered()
{
  AboutDialog dlg(this);
  dlg.exec();
}

void MainWindow::on_actionExit_triggered()
{
  if (m_PrefModel->mGameRunning)
  {
    int ret = QMessageBox::question(this, tr("BB Pref"),
          tr("Do you really want to quit the game?"),
          QMessageBox::Yes | QMessageBox::Default,
          QMessageBox::No | QMessageBox::Escape);
    if (ret == QMessageBox::No)
      return;
  }
  mDeskView->writeSettings();
  exit(0);
}
