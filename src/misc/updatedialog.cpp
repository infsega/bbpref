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

#include "updatedialog.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>

  UpdateDialog::UpdateDialog(QWidget *parent, const QString &updateText)
    : QDialog(parent)
  {
    this->setWindowTitle(tr("Updated Version of Avogadro Available",
                            "Indicates an updated Avogadro version is available"));

    QLabel *mainText = new QLabel(this);
    QPushButton *okButton = new QPushButton(tr("OK"), this);

    // The main label displaying update information
    mainText->setWordWrap(true);
    // Want to have links opened if they are in the release notes
    mainText->setOpenExternalLinks(true);
    // Set the supplied text
    mainText->setText(updateText);

    okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    okButton->setDefault(true);
    connect(okButton, SIGNAL(clicked()), this, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mainText);

    QHBoxLayout *okLayout = new QHBoxLayout();
    okLayout->addStretch();
    okLayout->addWidget(okButton);

    layout->addLayout(okLayout);
  }

  UpdateDialog::~UpdateDialog()
  {
  }



