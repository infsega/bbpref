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

#include <QDesktopWidget>
#include <QDialog>

#include "newgameform.h"

NewGameDialog::NewGameDialog (QWidget *parent) : QDialog(parent) {
  setupUi(this);
  setAttribute(Qt::WA_QuitOnClose, false);
  setAttribute(Qt::WA_DeleteOnClose, false);
  connect(cbRounds, SIGNAL(stateChanged(int)), this, SLOT(toggleRounds(int)));
}

void NewGameDialog::toggleRounds(int s) {
  sbRounds->setEnabled(s == Qt::Checked);
}
