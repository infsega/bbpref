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

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QRgb>

#include "ui_optform.h"

class OptionDialog : public QDialog, public Ui_OptDialog
{
  Q_OBJECT

public:
  explicit OptionDialog (QWidget *parent=0);
  ~OptionDialog () { }
  
  int backgroundType() const;
  void setBackgroundType(const int type);

  QRgb backgroundColor() const { return m_color; }
  void setBackgroundColor(const QRgb color);

  bool optionsModified() const;

private slots:
  void changeTexture(bool);
  void chooseColor();
  void toggleTakeQuality(int);

private:
  QRgb m_color;
  bool m_modified;
};


#endif
