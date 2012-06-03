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

#include "limits.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QLocale>
#include <QtCore/QTime>
#include <QTranslator>
#include <QDebug>

#include "debug.h"
#include "kpref.h"
#include "prfconst.h"

#ifdef USE_CONAN
  #include "Conan.h"
#endif

using namespace std;

int main (int argc, char *argv[]) {
  QCoreApplication::setOrganizationName("Inform Group");
  QCoreApplication::setOrganizationDomain("bbpref.card.game.com");
  QCoreApplication::setApplicationName("BB Pref");

#if !defined(WIN32) && !defined(_WIN32)
  char *e = getenv("DEBUG");
  if (!(!e || !strcmp(e, "0") || !strcasecmp(e, "off") || strcasecmp(e, "on"))) allowDebugLog = 1;
#endif

  for (int f = 1; f < argc; f++) {
    if (!strcmp(argv[f], "-d")) {
      for (int c = f; c < argc; c++) argv[c] = argv[c+1];
      argc--;
      f--;
      allowDebugLog = 1;
    }
  }

  const QTime t = QTime::currentTime();
  qsrand((double)t.minute()*t.msec()/(t.second()+1)*UINT_MAX/3600);

  QApplication app(argc, argv);

  char* country = NULL;
  char* language = NULL;

  bps_initialize();
  locale_get(&language, &country);
  QString translationCode = QString::fromAscii(language);
  //QString translationCode = QLocale::system().name();

  // Load Qt translations first
  QString qtFilename = "qt_" + translationCode + ".qm";
  QTranslator qtTranslator(0);
  if (qtTranslator.load(qtFilename, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    app.installTranslator(&qtTranslator);

  // Load translation of OpenPref
  QTranslator openprefTranslator(0);
  QString openprefFilename = ":/openpref_" + translationCode + ".qm";
  if(openprefTranslator.load(openprefFilename))
    app.installTranslator(&openprefTranslator);

  MainWindow kpref;
  kpref.showFullScreen();

  return app.exec();
}
