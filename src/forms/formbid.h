/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
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

#ifndef FORMBID_H
#define FORMBID_H

#include <QCloseEvent>
#include <QDialog>
#include <QList>
#include <QPushButton>
#include <QWidget>

#include "prfconst.h"
#include "qbidbutton.h"


class FormBid : public QDialog  {
 Q_OBJECT

public:
  FormBid (QWidget *parent=0);
  ~FormBid ();

  void enableAll ();
  void disableAll ();
  void disableLessThan (eGameBid);//(eGameBid);
  void disableItem (eGameBid);
  void enableItem (eGameBid);

public slots:
  void onBidClick (); // universal handler
  void slotPushButtonClick86 ();
  void slotPushButtonClickPass ();
  void slotPushButtonClickVist ();
  void slotPushButtonClickHalfVist ();
  void slotWithoutThree ();
  void slotShowBullet ();

protected:
  void initDialog ();
  QList<QPushButton *> buttonList ();
  QList<QBidButton *> bidButtonList ();

  void showEvent (QShowEvent *event);
  void closeEvent(QCloseEvent *event);

public:
  eGameBid _GamesType;
  QPushButton *bwithoutthree;
  QPushButton *showbullet;
  QPushButton *bhalfvist;
  
protected:
  QPushButton *b86;
  QPushButton *bpass;
  QPushButton *bvist;

};

extern FormBid *formBid;


#endif
