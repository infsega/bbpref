/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Mon Mar  6 14:43:31 EET 2000
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

#include <QApplication>

#include "prfconst.h"
#include "kpref.h"
#include "papplication.h"
#include "qtformtorg.h"

QApplication  *PQApplication;

int main(int argc, char *argv[]) {
  int RetVal;
  QApplication a(argc, argv);
  //a.setFont (QFont("helvetica", FONTSIZE));
  kpref=new Kpref();
  //!!!qApp->setMainWidget(kpref);
  Formtorg = new QTFormtorg(0, "Formtorg");
  Formtorg->hide();
  kpref->init();
  kpref->showMaximized();
  //!!!kpref-> resize(QApplication::desktop()->width()-30, QApplication::desktop()->height()-100);
  //kpref->show();
  kpref->adjustDesk();
  RetVal = a.exec();
  delete Formtorg;
  return RetVal;
}
