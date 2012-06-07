#pragma once

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class PrefModel;
class DeskView;

/**
 * @class MainWindow
 * @brief Main window of BB Pref
 *
 * Draws main window, creates PrefModel and DeskView and connects them
 * to helper dialogs
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT
    
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

public:
  PrefModel *m_PrefModel;
  DeskView *mDeskView;

private slots:
  void on_actionNewGame_triggered();
  void on_actionOpenGame_triggered();
  void on_actionSaveGame_triggered();
  void on_actionShowScore_triggered();
  void on_actionGameLog_triggered();
  void on_actionOptions_triggered();

  void on_actionRules_triggered();

  void on_actionAbout_triggered();

  void on_actionExit_triggered();

private:
  void adjustDesk();
  void doConnects();
  void readSettings();
  void writeSettings();

private:
  Ui::MainWindow *ui;
};
