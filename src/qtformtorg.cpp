/***************************************************************************
                          qtformtorg.cpp  -  description
                             -------------------
    begin                : Fri Mar 10 2000
    copyright            : (C) 2000 by Azarniy I.V.
    email                : azarniy@usa.net
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QObject>
#include "qtformtorg.h"
#include "prfconst.h"


QTFormtorg::QTFormtorg (QWidget *parent, const char *name) : QDialog (parent) {
  Q_UNUSED(name)
  initDialog();
}


QTFormtorg::~QTFormtorg () {
}


void QTFormtorg::slotPushButtonClick61 () { _GamesType = g61; accept(); }
void QTFormtorg::slotPushButtonClick62 () { _GamesType = g62; accept(); }
void QTFormtorg::slotPushButtonClick63 () { _GamesType = g63; accept(); }
void QTFormtorg::slotPushButtonClick64 () { _GamesType = g64; accept(); }
void QTFormtorg::slotPushButtonClick65 () { _GamesType = g65; accept(); }
void QTFormtorg::slotPushButtonClick71 () { _GamesType = g71; accept(); }
void QTFormtorg::slotPushButtonClick72 () { _GamesType = g72; accept(); }
void QTFormtorg::slotPushButtonClick73 () { _GamesType = g73; accept(); }
void QTFormtorg::slotPushButtonClick74 () { _GamesType = g74; accept(); }
void QTFormtorg::slotPushButtonClick75 () { _GamesType = g75; accept(); }
void QTFormtorg::slotPushButtonClick81 () { _GamesType = g81; accept(); }
void QTFormtorg::slotPushButtonClick82 () { _GamesType = g82; accept(); }
void QTFormtorg::slotPushButtonClick83 () { _GamesType = g83; accept(); }
void QTFormtorg::slotPushButtonClick84 () { _GamesType = g84; accept(); }
void QTFormtorg::slotPushButtonClick85 () { _GamesType = g85; accept(); }
void QTFormtorg::slotPushButtonClick86 () { _GamesType = g86; accept(); }
void QTFormtorg::slotPushButtonClick91 () { _GamesType = g91; accept(); }
void QTFormtorg::slotPushButtonClick92 () { _GamesType = g92; accept(); }
void QTFormtorg::slotPushButtonClick93 () { _GamesType = g93; accept(); }
void QTFormtorg::slotPushButtonClick94 () { _GamesType = g94; accept(); }
void QTFormtorg::slotPushButtonClick95 () { _GamesType = g95; accept(); }
void QTFormtorg::slotPushButtonClick101 () { _GamesType = g101; accept(); }
void QTFormtorg::slotPushButtonClick102 () { _GamesType = g102; accept(); }
void QTFormtorg::slotPushButtonClick103 () { _GamesType = g103; accept(); }
void QTFormtorg::slotPushButtonClick104 () { _GamesType = g104; accept(); }
void QTFormtorg::slotPushButtonClick105 () { _GamesType = g105; accept(); }
void QTFormtorg::slotPushButtonClickPass () { _GamesType = gtPass; accept(); }
void QTFormtorg::slotPushButtonClickVist () { _GamesType = vist; accept(); }
void QTFormtorg::slotGetBackSnos () { _GamesType = (TGamesType)0; reject(); }
void QTFormtorg::slotShowBullet () { _GamesType = (TGamesType)1; reject(); }


void QTFormtorg::EnableAll (void) {
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = dynamic_cast<QPushButton *>(widget);
    if (b) b->setEnabled(true);
  }
}


void QTFormtorg::DisalbeAll (void) {
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = dynamic_cast<QPushButton *>(widget);
    if (b) b->setEnabled(false);
  }
}

void QTFormtorg::DisableLessThan (TGamesType GamesType) {
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = dynamic_cast<QPushButton *>(widget);
    if (!b) continue;
    if (GamesTypeByName(b->text()) < GamesType) b->setEnabled(false);
  }
}


void QTFormtorg::DisalbeGames (TGamesType GamesType) {
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = dynamic_cast<QPushButton *>(widget);
    if (!b) continue;
    if (GamesTypeByName(b->text()) == GamesType) b->setEnabled(false);
  }
}


void QTFormtorg::EnableGames (TGamesType GamesType) {
  QList<QWidget *> wList = qFindChildren<QWidget *>(this);
  foreach (QWidget *widget, wList) {
    QPushButton *b = dynamic_cast<QPushButton *>(widget);
    if (!b) continue;
    if (GamesTypeByName(b->text()) == GamesType) b->setEnabled(true);
  }
}

QTFormtorg *Formtorg;
