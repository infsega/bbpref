/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      For more information, see <http://openpref.sourceforge.net/>
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

#include <bps/bps.h>
#include <bps/locale.h>

#include <limits>

#include <QApplication>
#include <QLibraryInfo>
#include <QtCore/QTime>
#include <QTranslator>
#include <QCleanlooksStyle>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("Inform Group");
  QCoreApplication::setOrganizationDomain("bbpref.card.game.com");
  QCoreApplication::setApplicationName("Pref");

  const QTime t = QTime::currentTime();
  qsrand((double)t.minute()*t.msec()/(t.second()+1)* std::numeric_limits<unsigned int>::max()/3600);

  QApplication app(argc, argv);
  app.setStyle( new QCleanlooksStyle() );

  char* country = NULL;
  char* language = NULL;
  bps_initialize();
  locale_get(&language, &country);
  QString translationCode = QString::fromAscii(language);

  // Load Qt translations first
  QString qtFilename = "qt_" + translationCode + ".qm";
  QTranslator qtTranslator(0);
  //QLibraryInfo::location(QLibraryInfo::TranslationsPath)
  if (qtTranslator.load(qtFilename, "app/native/translations"))
    app.installTranslator(&qtTranslator);

  // Load translation of OpenPref
  QTranslator openprefTranslator(0);
  QString openprefFilename = ":/bbpref_" + translationCode + ".qm";
  if(openprefTranslator.load(openprefFilename))
    app.installTranslator(&openprefTranslator);

  qDebug() << "Creating the main window " << endl;

  MainWindow pref;
  pref.showFullScreen();
  return app.exec();
}
