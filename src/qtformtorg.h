/***************************************************************************
                          qtformtorg.h  -  description
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
#ifndef QTFORMTORG_H
#define QTFORMTORG_H

#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QWidget>

#include "prfconst.h"
#include "qbidbutton.h"


class QTFormtorg : public QDialog  {
 Q_OBJECT

public:
  QTFormtorg (QWidget *parent=0);
  ~QTFormtorg ();

  void EnableAll (void);
  void DisalbeAll (void);
  void DisableLessThan (tGameBid);
  void DisalbeGames (tGameBid);
  void EnableGames (tGameBid);

public slots:
  void onBidClick (); // universal handler
  void slotPushButtonClick86 ();
  void slotPushButtonClickPass ();
  void slotPushButtonClickVist ();
  void slotGetBackSnos ();
  void slotShowBullet ();

protected:
  void initDialog ();
  QList<QPushButton *> buttonList ();
  QList<QBidButton *> bidButtonList ();

public:
  tGameBid _GamesType;
  QPushButton *bgetback;
  QPushButton *showbullet;

protected:
  QPushButton *b86;
  QPushButton *bpass;
  QPushButton *bvist;
};

extern QTFormtorg *Formtorg;


#endif
