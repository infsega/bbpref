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

#include <QMainWindow>


class DeskView;
class OptionDialog;
class PrefModel;
class UpdateCheck;

/**
 * @class MainWindow kpref.h
 * @brief Main window of OpenPref
 *
 * Draws main window, creates PrefModel and DeskView and connects them
 * to helper dialogs
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(bool fullScreen=false);
  ~MainWindow();

private slots:
  void newSingleGame ();
  void showScore ();
  void showLog();
  void showPlayers();
  void openFile ();
  void saveFile ();
  void helpAbout ();
  void helpRules ();
  void showOptions ();
  void applyOptions();

  bool quitGame ();

  void showHint(const QString & hint);
  void clearHint();
  void changeTitle(const QString & gameName);

private:
  void adjustDesk ();
  void initMenuBar();
  void writeSettings ();
  void readSettings ();
  void doConnects();

  QAction *actFileOpen;
  QAction *actFileSave;
  QStatusBar * HintBar;
  UpdateCheck * m_updateCheck;
  OptionDialog *m_optionDialog;
  bool m_fullScreen;

public:
  PrefModel *m_PrefModel;
  DeskView *mDeskView;

protected:
  void  keyPressEvent (QKeyEvent *);
  void  closeEvent(QCloseEvent *event);
};

#endif
