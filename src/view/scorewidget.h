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

class PrefModel;
class QPixmap;

class ScoreWidget : public QDialog
{
  Q_OBJECT

  public:
    ScoreWidget(PrefModel *model, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~ScoreWidget();
    QSize sizeHint() const;

  protected:
    void keyPressEvent (QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent (QPaintEvent *);
    void resizeEvent (QResizeEvent *);
    void showEvent(QShowEvent *event);

    void paintBlankPaper ();
    void showPlayerScore (int i, const QString scoreBullet, const QString scoreMountain,
      const QString scoreLeftWhist, const QString scoreRightWhist, const int scoreTotal);

    QPixmap *m_paperBmp;
    PrefModel *m_model;
};
