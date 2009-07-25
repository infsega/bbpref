/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2009 OpenPref Developers
 *      (see file AUTHORS for more details)
 *      Contact: annulen@users.sourceforge.net
 *      
 *      OpenPref is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
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

#include <time.h>

#include <QApplication>
#include <QTextCodec>

#include <QLocale>
#include <QTranslator>


#include "main.h"

#include "debug.h"
#include "prfconst.h"
#include "kpref.h"
#include "formbid.h"


using namespace std;

int main (int argc, char *argv[]) {
	/*
#if !defined(WIN32) && !defined(_WIN32)

  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("koi8-r"));
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("koi8-r"));
#else
  QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));
  QTextCodec::setCodecForLocale(QTextCodec::codecForName("windows-1251"));
#endif
*/
  QCoreApplication::setOrganizationName("Vampire Avalon");
  QCoreApplication::setOrganizationDomain("ketmar.no-ip.org"); // fuck macz
  QCoreApplication::setApplicationName("OpenPref");

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
    } else if (!strcmp(argv[f], "-o")) {
		optAlphaBeta1 = false;
		optAlphaBeta2 = false;
	}
  }

  qsrand((unsigned)time(0));

  QApplication a(argc, argv);
  QString translationCode = QLocale::system().name();
  QString qtFilename = "./openpref_" + translationCode + ".qm";
  //cout << "Loading translation " << qtFilename << " ...\n";
  QTranslator qtTranslator(0);
  qtTranslator.load(qtFilename);
  a.installTranslator(&qtTranslator);

  //a.setFont (QFont("helvetica", FONTSIZE));
  kpref = new Kpref();
  formBid = new FormBid(kpref);
  formBid->hide();
  kpref->init();
  kpref->show();
  return a.exec();
}
