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

class PrefModel;
class DeskView;
class UpdateCheck;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(bool fullScreen=false);
  ~MainWindow();

public slots:
  void newSingleGame ();
  void showScore ();
  void showPlayers();
  void openFile ();
  void saveFile ();
  void helpAbout ();
  void helpRules ();
  void options ();

  bool quitGame ();

  void showHint(QString hint);
  void clearHint();

private:
  void adjustDesk ();
  void initMenuBar();
  void writeSettings ();
  void readSettings ();
  void doConnects();
  void deckChanged();

  QAction *actFileOpen;
  QAction *actFileSave;
  QStatusBar * HintBar;
  UpdateCheck * m_updateCheck;
  bool m_fullScreen;

public:
  PrefModel *m_PrefModel;
  DeskView *mDeskView;

protected:
  void  keyPressEvent (QKeyEvent *);
  void  closeEvent(QCloseEvent *event);
};

#endif
