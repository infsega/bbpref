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

#include "prfconst.h"
#include "scorewidget.h"

#include <QBrush>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QRadialGradient>

#include "desktop.h"
#include "player.h"

static void drawRotatedText (QPainter &p, int x, int y, float angle, const QString &text) {
  p.translate(x, y);
  p.rotate(angle);
  p.drawText(0, 0, text);
  p.rotate(-1*angle);
  p.translate(-1*x, -1*y);
}

static void drawRotatedText (QPainter &p, int x, int y, int width, int height, float angle, const QString &text) {
  p.translate(x, y);
  p.rotate(angle);
  p.drawText(QRectF(0,0,width,height),text,QTextOption(Qt::AlignHCenter));
  //p.drawRect(0,0,width,height);
  p.rotate(-1*angle);
  p.translate(-1*x, -1*y);
}

static void paintBlankPaper (QPixmap *paper) {
  const int PaperWidth = paper->width();
  const int PaperHeight = paper->height();
  const int PoolWidth = 60;
  const int Pool2Width = 85;
  const int maxPoolRadius = 20;
  
  // if (width() > height()) horizontal

  QPainter p(paper);
  p.setRenderHints(QPainter::Antialiasing);
  QRect NewRect = QRect(0, 0, PaperWidth, PaperHeight);
  QBrush brush(qRgb(255, 255, 255));
  p.fillRect(NewRect, brush);
  p.setPen(Qt::black);

  // Draw borders of paper
  p.drawRect(NewRect);
  //QPoint center(NewRect.center().x(), NewRect.center().y()*1.23);
  QPoint center(PaperWidth/2, PaperHeight-PaperWidth/2);

  // Diagonal lines from bottom corners to circle
  p.drawLine(NewRect.bottomLeft(), center);
  p.drawLine(NewRect.bottomRight(), center);

  // Central vertical line
  p.drawLine( center.x(), 0, center.x(), center.y() );

  // External border of pool
  p.drawRect(PoolWidth, 0, PaperWidth-2*PoolWidth, PaperHeight-PoolWidth);

  // Border of mountain
  p.drawRect(Pool2Width, 0, PaperWidth-2*Pool2Width, PaperHeight-Pool2Width);

  // Player lines
  p.drawLine(0, (PaperHeight-PoolWidth)/2, PoolWidth, (PaperHeight-PoolWidth)/2);
  p.drawLine(PaperWidth, (PaperHeight-PoolWidth)/2, PaperWidth-PoolWidth, (PaperHeight-PoolWidth)/2);
  p.drawLine(PaperWidth/2, PaperHeight, PaperWidth/2, PaperHeight-PoolWidth);

  // Circle with MaxPool value
  QRadialGradient g(center, maxPoolRadius, center+QPoint(-maxPoolRadius/2,-maxPoolRadius/2));
  g.setColorAt(0, Qt::white);
  g.setColorAt(1, qRgb(250, 250, 0));
  //g.setColorAt(1, Qt::gray);
  //QBrush b1(brush);
  //b1.setColor(qRgb(255, 255, 0));
  QBrush b1(g);
  p.setBrush(b1);
  p.drawEllipse(center, maxPoolRadius, maxPoolRadius);

  // Draw text  
  // MaxPool
  QFont fnt(p.font());
  fnt.setBold(true);
  p.setFont(fnt);
  p.drawText(QRect(center.x() - maxPoolRadius, center.y() - maxPoolRadius,
    maxPoolRadius*2, maxPoolRadius*2), QString::number(optMaxPool), QTextOption(Qt::AlignCenter));
  fnt.setBold(false);
  p.setFont(fnt);
  
  // Players' names
  p.setBrush(brush);
  const QRect r1 = p.boundingRect(NewRect, Qt::AlignHCenter, optHumanName);
  const QRect r2 = p.boundingRect(NewRect, Qt::AlignHCenter, optPlayerName1);
  const QRect r3 = p.boundingRect(NewRect, Qt::AlignHCenter, optPlayerName2);
  p.drawText(QRect(center.x()-r1.width()/2, center.y()+55, r1.width(), r1.height()),
    optHumanName, QTextOption(Qt::AlignHCenter));
  drawRotatedText(p, center.x() - 30, (PaperHeight - Pool2Width - r2.width())/2,
    r2.width(), r2.height(), 90,optPlayerName1);
  drawRotatedText(p, center.x() + 30, (PaperHeight - Pool2Width + r3.width())/2,
    r3.width(), r3.height(), -90,optPlayerName2);
  
  p.end();
}

ScoreWidget::ScoreWidget(PrefDesktop *desktop, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), m_desktop(desktop), m_paperBmp(0)
{
  // TODO: flexible size
  //setFixedSize(410,530);
  setFixedSize(410,480);
  setWindowTitle(tr("Score"));
  m_paperBmp = new QPixmap(width(), height());
  paintBlankPaper(m_paperBmp);
}


ScoreWidget::~ScoreWidget()
{}

void ScoreWidget::keyPressEvent (QKeyEvent *event)
{
    switch (event->key()) {
      case Qt::Key_Escape:
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Space:
        hide();
        break;
      default: ;
    }
}

void ScoreWidget::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  hide();
}

void ScoreWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event)

  QString sb, sm, slw, srw;
  int tw;
    QPainter p;
    p.begin(this);
    p.drawPixmap(0, 0, *(m_paperBmp));

  for (int i = 1;i<=3;i++) {
    Player *plr = m_desktop->player(i);
    sb = plr->mScore.poolStr();
    sm = plr->mScore.mountainStr(7);
    slw = plr->mScore.leftWhistsStr(14);
    srw = plr->mScore.rightWhistsStr(14);
    tw = plr->mScore.whists();
    showPlayerScore(i, sb, sm, slw, srw, tw);
  }

  p.end();
}

void ScoreWidget::showPlayerScore (int i, const QString scoreBullet, const QString scoreMountain,
  const QString scoreLeftWhist, const QString scoreRightWhist, const int scoreTotal)
{
  const int PoolWidth = 60;
  const int Pool2Width = 85;
  const int maxPoolRadius = 20;
  //const int margin = 4;
  const int PaperWidth = m_paperBmp->width();
  const int PaperHeight = m_paperBmp->height();
  QPoint center(width()/2, height()-width()/2);
  QPainter p(this);
  p.setPen(qRgb(0, 0, 0));
  QFont fnt(p.font());
  //fnt.setBold(false);
  //p.setFont(fnt);
  QRect r1 = p.boundingRect(rect(), Qt::AlignHCenter, QString::number(scoreTotal));
  switch (i) {
    case 1:
      // Bullet
      p.setPen(qRgb(0, 128, 0));
      p.drawText(PoolWidth+r1.height()+4, PaperHeight-PoolWidth-4, scoreBullet);
      // Mountain
      p.setPen(qRgb(235, 0, 0));
      p.drawText(Pool2Width+r1.height()+4, PaperHeight-Pool2Width-4, scoreMountain);
      // Whists
      p.setPen(qRgb(0, 0, 0));
      p.drawText(r1.height()+4, PaperHeight-4, scoreLeftWhist);
      p.drawText(center.x()+4, PaperHeight-4, scoreRightWhist);
      // Total score
      if (scoreTotal >= 0)
        p.setPen(qRgb(0, 0, 235));
      else
        p.setPen(qRgb(235, 0, 0));
      fnt.setBold(true);
      p.setFont(fnt);
      p.drawText(center.x()-r1.width()/2, center.y()+80, QString::number(scoreTotal));
      break;
    case 2:
      // Bullet
      p.setPen(qRgb(0, 128, 0));
      drawRotatedText(p, PoolWidth+4, 4, 90, scoreBullet);
      // Mountain
      p.setPen(qRgb(235, 0, 0));
      drawRotatedText(p, Pool2Width+4, 4, 90, scoreMountain);
      // Whists
      p.setPen(qRgb(0, 0, 0));
      drawRotatedText(p, 4, 4, 90, scoreLeftWhist);
      drawRotatedText(p, 4, (PaperHeight-PoolWidth)/2+4, 90, scoreRightWhist);
      // Total score
      if (scoreTotal >= 0)
        p.setPen(qRgb(0, 0, 235));
      else
        p.setPen(qRgb(235, 0, 0));
      fnt.setBold(true);
      p.setFont(fnt);
      drawRotatedText(p, center.x() - 60, (PaperHeight - Pool2Width - r1.width())/2,
        90, QString::number(scoreTotal));
      break;
    case 3:
      // Bullet
      p.setPen(qRgb(0, 128, 0));
      drawRotatedText(p, PaperWidth-PoolWidth-4, PaperHeight-PoolWidth-r1.height(), -90, scoreBullet);
      // Mountain
      p.setPen(qRgb(235, 0, 0));
      drawRotatedText(p, PaperWidth-Pool2Width-4, PaperHeight-Pool2Width-r1.height(), -90, scoreMountain);
      // Whists
      p.setPen(qRgb(0, 0, 0));
      drawRotatedText(p, PaperWidth-4, PaperHeight-r1.height()-4, -90, scoreLeftWhist);
      drawRotatedText(p, PaperWidth-4, (PaperHeight-PoolWidth)/2-4, -90, scoreRightWhist);
      // Total score
      if (scoreTotal >= 0)
        p.setPen(qRgb(0, 0, 235));
      else
        p.setPen(qRgb(235, 0, 0));
      fnt.setBold(true);
      p.setFont(fnt);
      drawRotatedText(p, center.x() + 60, (PaperHeight - Pool2Width + r1.width())/2,
        -90, QString::number(scoreTotal));
      break;
    default: ;
  }
  fnt.setBold(false);
  p.setFont(fnt);
  p.end();
}
