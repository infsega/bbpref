/* coded by Ketmar // Avalon Group (psyc://ketmar.no-ip.org/~Ketmar)
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
#ifndef ACCWIN_H
#define ACCWIN_H

#include <QDialog>


#include "ui_optform.h"
class OptDialog : public QDialog, public Ui_OptDialog {
  Q_OBJECT

public:
  OptDialog (QWidget *parent=0);
  ~OptDialog () { }

protected:
  void showEvent (QShowEvent *event);
};


#endif
