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

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QSettings>
#include <QtCore/QDebug>

namespace {
  class UpdateDialog : public QDialog
  {
  public:
    UpdateDialog(QWidget *parent, const QString &updateText) : QDialog(parent)
    {
      setMinimumWidth(275);
      setWindowTitle(tr("Updated Version of OpenPref Available",
                              "Indicates an updated OpenPref version is available"));

      QLabel *mainText = new QLabel(this);
      QPushButton *okButton = new QPushButton(tr("OK"), this);
      QPushButton *remindButton = new QPushButton(tr("Remind Later"), this);

      // The main label displaying update information
      mainText->setWordWrap(true);
      // Want to have links opened if they are in the release notes
      mainText->setOpenExternalLinks(true);
      // Set the supplied text
      mainText->setText(updateText);

      okButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      okButton->setDefault(true);
      remindButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
      connect(remindButton, SIGNAL(clicked()), this, SLOT(reject()));

      QVBoxLayout *layout = new QVBoxLayout(this);
      layout->addWidget(mainText);

      QHBoxLayout *okLayout = new QHBoxLayout();
      okLayout->addStretch();
      okLayout->addWidget(okButton);
      okLayout->addWidget(remindButton);
      okLayout->addStretch();

      layout->addLayout(okLayout);
    }
  };
}

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
      if (info->exec() == QDialog::Accepted) {
        // Now we have warned the user, set this version as the prompted version
        m_versionPrompted = version;
      }
      delete info;
    }
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
