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

#include "prfconst.h"
#include "updatecheck.h"
#include "updatedialog.h"

#include <QtGui/QMessageBox>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QSettings>
#include <QtCore/QDebug>


  UpdateCheck * UpdateCheck::instance(QWidget* parent)
  {
    static UpdateCheck * obj = 0;
    if (!obj) {
      obj = new UpdateCheck(parent);
    }
    return obj;
  }

  UpdateCheck::UpdateCheck(QWidget* parent) : QWidget(parent), m_network(0)
  {
  }

  UpdateCheck::~UpdateCheck()
  {
  }

  void UpdateCheck::writeSettings(QSettings &settings) const
  {
    settings.setValue("update/versionPrompted", m_versionPrompted);
  }

  void UpdateCheck::readSettings(const QSettings &settings)
  {
    m_versionPrompted = settings.value("update/versionPrompted",
                                        OPENPREF_VERSION).toString();
  }

  void UpdateCheck::checkForUpdates()
  {
    if (!m_network) {
      m_network = new QNetworkAccessManager(this);
      connect(m_network, SIGNAL(finished(QNetworkReply*)),
              this, SLOT(replyFinished(QNetworkReply*)));
    }

    // Check the URL for the latest release version of OpenPref
    m_network->get(QNetworkRequest(QUrl("http://openpref.sourceforge.net/version.txt")));
  }

  void UpdateCheck::replyFinished(QNetworkReply *reply)
  {
    // Read in all the data
    if (!reply->isReadable()) {
      QMessageBox::warning(qobject_cast<QWidget*>(parent()),
                           tr("Network Update Check Failed"),
                           tr("Network timeout or other error."));
      reply->deleteLater();
      return;
    }

    QString version, releaseNotes;
    bool newVersionAvailable = false;

    // reply->canReadLine() always returns false, so this seems to best approach
    QStringList lines = QString(reply->readAll()).split('\n');
    for(int i = 0; i < lines.size(); ++i) {
      if (lines[i] == "[Version]" && lines.size() > ++i) {
        version = lines[i];
        if (versionCompare(version))
          newVersionAvailable = true;
      }
      if (lines[i] == "[Release Notes]" && lines.size() > ++i) {
        // Right now just reading in the rest of the file as release notes
        for (int j = i-1; j >=0; --j)
          lines.removeAt(j);
        releaseNotes = lines.join("\n");
      }
    }

    if (newVersionAvailable) {
      //QPointer<UpdateDialog> info = new UpdateDialog(qobject_cast<QWidget *>(parent()), releaseNotes);
      UpdateDialog* info = new UpdateDialog(qobject_cast<QWidget*>(parent()), releaseNotes);
      info->exec();
      delete info;
    }
    // Now we have warned the user, set this version as the prompted version
    m_versionPrompted = version;

    // We are responsible for deleting the reply object
    reply->deleteLater();
  }

  bool UpdateCheck::versionCompare(const QString& newVersion)
  {
    QStringList newParts = newVersion.split('.');
    if (m_versionPrompted.isEmpty())
      m_versionPrompted = OPENPREF_VERSION;
    QStringList oldParts = m_versionPrompted.split('.');

    if (newParts.size() < 3 || oldParts.size() < 3) {
      qDebug() << "Error size of new or old version strings is too small:"
          << OPENPREF_VERSION << newVersion;
    }
    int parts = newParts.size() < oldParts.size() ? newParts.size()
                                                  : oldParts.size();

    // Now compare each part of the version - return on the first that is bigger
    for (int i = 0; i < parts; ++i) {
      if (newParts[i] > oldParts[i])
        return true;
      else if (newParts[i] < oldParts[i]) // Should never happen, good to check though
        return false;
    }
    return false;
  }
