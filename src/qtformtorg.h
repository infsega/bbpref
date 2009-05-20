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

#include <QWidget>
#include <QPushButton>

#include <QDialog>
#include "prfconst.h"


class QTFormtorg : public QDialog  {
 Q_OBJECT

public:
  QTFormtorg (QWidget *parent=0, const char *name=0);
  ~QTFormtorg ();

  TGamesType _GamesType;

public slots:
  void slotPushButtonClick61 ();
  void slotPushButtonClick62 ();
  void slotPushButtonClick63 ();
  void slotPushButtonClick64 ();
  void slotPushButtonClick65 ();
  void slotPushButtonClick71 ();
  void slotPushButtonClick72 ();
  void slotPushButtonClick73 ();
  void slotPushButtonClick74 ();
  void slotPushButtonClick75 ();
  void slotPushButtonClick81 ();
  void slotPushButtonClick82 ();
  void slotPushButtonClick83 ();
  void slotPushButtonClick84 ();
  void slotPushButtonClick85 ();
  void slotPushButtonClick86 ();
  void slotPushButtonClick91 ();
  void slotPushButtonClick92 ();
  void slotPushButtonClick93 ();
  void slotPushButtonClick94 ();
  void slotPushButtonClick95 ();
  void slotPushButtonClick101 ();
  void slotPushButtonClick102 ();
  void slotPushButtonClick103 ();
  void slotPushButtonClick104 ();
  void slotPushButtonClick105 ();
  void slotPushButtonClickPass ();
  void slotPushButtonClickVist ();
  void slotGetBackSnos ();
  void slotShowBullet ();

public:
  QPushButton *bgetback;
  QPushButton *showbullet;

protected:
  void initDialog ();
  //Generated area. DO NOT EDIT!!!(begin)
  QPushButton *b61;
  QPushButton *b62;
  QPushButton *b63;
  QPushButton *b64;
  QPushButton *b71;
  QPushButton *b81;
  QPushButton *b91;
  QPushButton *b101;
  QPushButton *b72;
  QPushButton *b82;
  QPushButton *b92;
  QPushButton *b102;
  QPushButton *b73;
  QPushButton *b83;
  QPushButton *b93;
  QPushButton *b103;
  QPushButton *b74;
  QPushButton *b84;
  QPushButton *b94;
  QPushButton *b104;
  QPushButton *bpass;
  QPushButton *bvist;
  QPushButton *b86;
  QPushButton *b65;
  QPushButton *b75;
  QPushButton *b85;
  QPushButton *b95;
  QPushButton *b105;
  //Generated area. DO NOT EDIT!!!(end)

public:
  void EnableAll (void);
  void DisalbeAll (void);
  void DisableLessThan (TGamesType);
  void DisalbeGames (TGamesType);
  void EnableGames (TGamesType);
};

extern QTFormtorg *Formtorg;


#endif
