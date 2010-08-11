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

#include "optiondialog.h"
#include <QtGui/QColorDialog>

OptionDialog::OptionDialog (QWidget *parent) : QDialog(parent), m_color(qRgb(0,128,0))
{
  setupUi(this);
  changeTexture(true);
  connect(rbTexture0, SIGNAL(toggled(bool)), this, SLOT(changeTexture(bool)));
  connect(rbTexture1, SIGNAL(toggled(bool)), this, SLOT(changeTexture(bool)));
  connect(rbTexture2, SIGNAL(toggled(bool)), this, SLOT(changeTexture(bool)));
}

int OptionDialog::backgroundType() const
{
  if (rbTexture0->isChecked())
    return 0;
  else if (rbTexture1->isChecked())
    return 1;
  else if (rbTexture2->isChecked())
    return 2;
  else
    return 1;
}

void OptionDialog::setBackgroundType(const int type)
{
  switch (type) {
    case 0:
      rbTexture0->toggle();
      break;
    case 1:
      rbTexture1->toggle();
      break;
    case 2:
      rbTexture2->toggle();
      break;
  }
  changeTexture(true);
}

void OptionDialog::setBackgroundColor(const QRgb color)
{
  m_color = color;
  changeTexture(true);
}

void OptionDialog::changeTexture(bool)
{
  if (rbTexture0->isChecked()) {
    // Pure Color
    QPixmap p(80,60);
    p.fill(m_color);
    colorButton->setIcon(p);
    colorButton->setFlat(false);
    disconnect(colorButton, 0, 0, 0);
    connect(colorButton, SIGNAL(clicked()), this, SLOT(chooseColor()));
    
  } else if (rbTexture1->isChecked()) {
    // Cloth
    disconnect(colorButton, 0, 0, 0);
    colorButton->setFlat(true);
    colorButton->setIcon(QPixmap(":/pics/cloth_s.png"));
    
  } else if (rbTexture2->isChecked()) {
    // Wood
    disconnect(colorButton, 0, 0, 0);
    colorButton->setFlat(true);
    colorButton->setIcon(QPixmap(":/pics/wood_s.jpg"));
  }
}

void OptionDialog::chooseColor()
{
  QColor color = QColorDialog::getColor(m_color, 0, tr("Choose background color"));
  if (color.isValid() && color.rgb() != m_color)
    m_color = color.rgb();
  changeTexture(true);
}
