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

#include "playersinfodialog.h"
#include "desktop.h"
#include "player.h"

#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QPixmap>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QLabel>

PlayersInfoDialog::PlayersInfoDialog(PrefModel *model,
                                    QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
  setWindowTitle(tr("Players"));
  const int nFields = 2;
  QGridLayout *layout = new QGridLayout(this);
  // m_model->numOfPlayers ?
  for (int i=1; i<=3; i++) {
    Player *p = model->player(i);
    QLabel *nick = new QLabel(QString("<b>") + tr("Name") + "</b>" + ": " + p->nick(), this);
    QLabel *avatar = new QLabel(this);
    QLabel *type;
    if (p->isHuman()) {
      type = new QLabel(p->type(), this);
      avatar->setPixmap(QPixmap(":/pics/unknown-player.png"));
    } else {
      type = new QLabel(QString("<b>") + tr("AI") + "</b> (<i>" + p->type() + "</i>)", this);
      avatar->setPixmap(QPixmap(":/pics/ai-player.png"));
    }
    //QLabel *num = new QLabel(QString::number(p->number()), this);
    QFrame *separator = new QFrame(this);
    separator->setFrameStyle(QFrame::HLine);
    layout->addWidget(avatar, (i-1)*(nFields+1), 0, nFields, 1, Qt::AlignCenter);
    layout->addWidget(nick, (i-1)*(nFields+1), 1);//, nFields, 1, Qt::AlignCenter);
    layout->addWidget(type, (i-1)*(nFields+1) + 1, 1);
    //layout->addWidget(num, (i-1)*nFields+2, 1);
    layout->addWidget(separator, (i-1)*(nFields+1) + 2, 0, 1, -1);//, Qt::AlignCenter);
    resize(400,250);
  }  
}


void PlayersInfoDialog::keyPressEvent (QKeyEvent *event)
{
    switch (event->key()) {
      case Qt::Key_Escape:
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Space:
        hide();
        break;
      default: ;
    }
}

void PlayersInfoDialog::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  hide();
}
