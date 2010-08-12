/*
 *      OpenPref - cross-platform Preferans game
 *
 *      This file was taken from Avogadro project
 *      <http://avogadro.openmolecules.net/>
 *      Copyright (C) 2009 Marcus D. Hanwell
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

#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QtGui/QDialog>


  class UpdateDialog : public QDialog
  {
    Q_OBJECT

  public:
    UpdateDialog(QWidget *parent, const QString &updateText);
    ~UpdateDialog();
  };

#endif // UPDATEDIALOG_H
