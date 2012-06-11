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

#pragma once

#include <QDialog>
namespace Ui
{
  class NewGameDialog;
}

class PrefModel;

class NewGameDialog : public QDialog
{
  Q_OBJECT

public:
  NewGameDialog (PrefModel* i_model, QWidget *parent = NULL);
  ~NewGameDialog();

  void copyOptions(PrefModel& o_model);

protected:
  void showEvent(QShowEvent *);

private slots:
  void toggleRounds(int checked);

  void on_buttonBox_accepted();

private:
  Ui::NewGameDialog *ui;
  PrefModel* model;
};
