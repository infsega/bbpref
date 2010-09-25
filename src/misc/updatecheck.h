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

#ifndef UPDATECHECK_H
#define UPDATECHECK_H

#include <QtGui/QWidget>

class QNetworkAccessManager;
class QNetworkReply;
class QString;
class QSettings;

  class UpdateCheck : public QWidget
  {
  Q_OBJECT

  public:
    static UpdateCheck * instance(QWidget* parent = 0);
    ~UpdateCheck();

    ///Save settings
    void writeSettings(QSettings &settings) const;
    ///Read settings
    void readSettings(const QSettings &settings);

  public slots:
    ///Check for available application updates
    void checkForUpdates();

  private:
    UpdateCheck(QWidget* parent = 0); // Private constructor - singleton
    UpdateCheck(UpdateCheck const&); // Private copy constructor
    UpdateCheck& operator=(UpdateCheck const&); // Private assignment operator

    QNetworkAccessManager *m_network;
    QString m_versionPrompted;
    bool m_interactive;

    /**
     * Compare the two versions, passed in the form of x.y.z.
     * @param newVersion The new version to compare to the library version.
     * @return True if the new version is greater than the library version.
     */
    bool versionCompare(const QString& newVersion);

  private slots:
    void replyFinished(QNetworkReply*);

  };


#endif // UPDATECHECK_H
