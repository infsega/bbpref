/*
 *      OpenPref - cross-platform Preferans game
 *      
 *      Copyright (C) 2000-2010 OpenPref Developers
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

#include <QDialog>

class PrefDesktop;
class QPixmap;

class ScoreWidget : public QDialog
{
  Q_OBJECT

  public:
    ScoreWidget(PrefDesktop *desktop, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ScoreWidget();

  protected:
    void keyPressEvent (QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent (QPaintEvent *);
    void paintBlankPaper();
    void showPlayerScore (int i, const QString &sb, const QString &sm, const QString &slv, const QString &srv, const QString &tv);

    QPixmap *m_paperBmp;
    PrefDesktop *m_desktop;
};
