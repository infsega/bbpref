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

#include "helpbrowser.h"

HelpBrowser::HelpBrowser (QWidget *parent) : QDialog(parent) {
  setupUi(this);
  setAttribute(Qt::WA_QuitOnClose, false);
  setAttribute(Qt::WA_DeleteOnClose, false);
}


void HelpBrowser::showEvent (QShowEvent *event) {
  Q_UNUSED(event)
  QWidget *parentObj = dynamic_cast<QWidget *>(parent());
  int x, y;
  if (parentObj) {
    QRect dims(parentObj->frameGeometry());
    x = dims.left()+(dims.width()-width())/2;
    y = dims.top()+(dims.height()-height())/2;
  } else {
    QDesktopWidget *desk = QApplication::desktop();
    QRect dims(desk->availableGeometry(this));
    x = dims.left()+(dims.width()-width())/2;
    y = dims.top()+(dims.height()-height())/2;
  }
  move(x, y);
}
