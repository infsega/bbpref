/* coded by Ketmar // Avalon Group (psyc://ketmar.no-ip.org/~Ketmar)
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
#include <QDesktopWidget>
#include <QDialog>

#include "optform.h"

OptDialog::OptDialog (QWidget *parent) : QDialog(parent) {
  setupUi(this);
  setAttribute(Qt::WA_QuitOnClose, false);
  setAttribute(Qt::WA_DeleteOnClose, false);
}


void OptDialog::showEvent (QShowEvent *event) {
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
