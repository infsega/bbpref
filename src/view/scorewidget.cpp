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

ScoreWidget::ScoreWidget(PrefModel *model, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f), m_model(model), m_paperBmp(0)
{
  // TODO: flexible size
  setFixedSize(410,530);
  setWindowTitle(tr("Score"));
  m_paperBmp = new QPixmap(width(), height());
  paintBlankPaper();
}

void ScoreWidget::paintBlankPaper () {
  const int PaperWidth = width();
  const int PaperHeight = height();
  const int PoolWidth = 40;
  const int xDelta = 0; //(width()-PaperWidth)/2;
  const int yDelta = 0; //(height()-PaperHeight)/2;

  // if (width() > height()) horizontal

  QPainter p(m_paperBmp);
  QRect NewRect = QRect(xDelta, yDelta, PaperWidth, PaperHeight);
  QBrush brush(qRgb(255, 255, 255));
  p.fillRect(NewRect, brush);
  QBrush b1(brush);
  b1.setColor(qRgb(255, 255, 0));
  p.setBrush(b1);
  p.setPen(qRgb(0, 0, 0));

  // Draw borders of paper
  p.drawLine(xDelta, yDelta, xDelta+PaperWidth, yDelta);
  p.drawLine(xDelta, yDelta, xDelta, yDelta+PaperHeight);
  p.drawLine(xDelta, yDelta+PaperHeight, xDelta+PaperWidth, yDelta+PaperHeight);
  p.drawLine(xDelta+PaperWidth, yDelta, xDelta+PaperWidth, yDelta+PaperHeight);

  // Circle with MaxPool value
  p.drawEllipse(xDelta+PaperWidth/2-18,yDelta+277,36,36);

  // Diagonal lines from bottom corners to circle
  p.drawLine(xDelta+PaperWidth, yDelta+PaperHeight, xDelta+PaperWidth-190, yDelta+307);
  p.drawLine(xDelta+0, yDelta+PaperHeight, xDelta+190, yDelta+307);

  // Central vertical line
  p.drawLine(xDelta+PaperWidth/2, yDelta+277, xDelta+PaperWidth/2, yDelta+0);

  // External border of pool
  p.drawLine(xDelta+PoolWidth, yDelta+0, xDelta+PoolWidth, yDelta+483);
  p.drawLine(xDelta+PoolWidth, yDelta+483, xDelta+370, yDelta+483);
  p.drawLine(xDelta+370, yDelta+483, xDelta+370, yDelta+0);
  //p.drawLine(xDelta+370, yDelta+0, xDelta+80, yDelta+0);
  
  // Border of mountain
  p.drawLine(xDelta+2*PoolWidth, yDelta+0, xDelta+2*PoolWidth, yDelta+436);
  p.drawLine(xDelta+2*PoolWidth, yDelta+436, xDelta+330, yDelta+436);
  p.drawLine(xDelta+330, yDelta+436, xDelta+330, yDelta+0);
  
  p.drawLine(xDelta+0, yDelta+255, xDelta+PoolWidth, yDelta+255);
  p.drawLine(xDelta+PaperWidth, yDelta+255, xDelta+370, yDelta+255);
  p.drawLine(xDelta+PaperWidth/2, yDelta+PaperHeight, xDelta+PaperWidth/2, yDelta+483);

  // Draw text
  
  // MaxPool
  //p.drawText(xDelta+197, FONTSIZE+yDelta+292, buff);
  p.drawText(QRect(xDelta+PaperWidth/2-18,yDelta+277,36,36),
  QString::number(optMaxPool), QTextOption(Qt::AlignCenter));
  
  // Players' names
  p.setBrush(brush);
  p.drawText(QRect(xDelta+150,yDelta+350,110,20),optHumanName,QTextOption(Qt::AlignHCenter));
  drawRotatedText(p,xDelta+175,yDelta+200,110,20,90,optPlayerName1);
  drawRotatedText(p,xDelta+235,yDelta+310,110,20,-90,optPlayerName2);
  
  p.end();

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

  QString sb, sm, slw, srw, tw;
    QPainter p;
    p.begin(this);
    p.drawPixmap(0, 0, *(m_paperBmp));

  for (int i = 1;i<=3;i++) {
    Player *plr = m_model->player(i);
    sb = plr->mScore.poolStr();
    sm = plr->mScore.mountainStr(7);
    slw = plr->mScore.leftWhistsStr(14);
    srw = plr->mScore.rightWhistsStr(14);
    tw = plr->mScore.whistsStr();
    showPlayerScore(i, sb, sm, slw, srw, tw);
  }

    p.end();
}

void ScoreWidget::showPlayerScore (int i, const QString &sb, const QString &sm, const QString &slv, const QString &srv, const QString &tv) {
  const int xDelta = 0; //(width()-PaperWidth)/2;
  const int yDelta = 0;
  QPainter p(this);
  p.setPen(qRgb(0, 0, 0));
  QFont fnt(p.font());
  fnt.setBold(false);
  p.setFont(fnt);
  switch (i) {
    case 1:
      p.setPen(qRgb(0, 128, 0));
      p.drawText(xDelta+65, FONTSIZE+yDelta+450, sb);
      p.setPen(qRgb(255, 0, 0));
      p.drawText(xDelta+105, FONTSIZE+yDelta+420, sm);
      p.setPen(qRgb(0, 0, 0));
      p.drawText(xDelta+15, FONTSIZE+yDelta+510, slv);
      p.drawText(xDelta+220, FONTSIZE+yDelta+510, srv);
      p.setPen(qRgb(0, 0, 255));
      fnt.setBold(true);
      p.setFont(fnt);
      p.drawText(xDelta+188, FONTSIZE+yDelta+390, tv);
      break;
    case 2:
      p.setPen(qRgb(0, 128, 0));
      drawRotatedText(p, xDelta+60, FONTSIZE+yDelta+5, 90, sb);
      p.setPen(qRgb(255, 0, 0));
      drawRotatedText(p, xDelta+100, FONTSIZE+yDelta+5, 90, sm);
      p.setPen(qRgb(0, 0, 0));
      drawRotatedText(p, xDelta+18, FONTSIZE+yDelta+5, 90, slv);
      drawRotatedText(p, xDelta+18, FONTSIZE+yDelta+270, 90, srv);
      p.setPen(qRgb(0, 0, 255));
      fnt.setBold(true);
      p.setFont(fnt);
      drawRotatedText(p, xDelta+170, FONTSIZE+yDelta+120, 90, tv);
      break;
    case 3:
      p.setPen(qRgb(0, 128, 0));
      drawRotatedText(p, xDelta+355, FONTSIZE+yDelta+440, -90, sb);
      p.setPen(qRgb(255, 0, 0));
      drawRotatedText(p, xDelta+322, FONTSIZE+yDelta+405, -90, sm);
      p.setPen(qRgb(0, 0, 0));
      drawRotatedText(p, xDelta+400, FONTSIZE+yDelta+500, -90, slv);
      drawRotatedText(p, xDelta+400, FONTSIZE+yDelta+240, -90, srv);
      p.setPen(qRgb(0, 0, 255));
      fnt.setBold(true);
      p.setFont(fnt);
      drawRotatedText(p, xDelta+250, FONTSIZE+yDelta+120, -90, tv);
      break;
    default: ;
  }
  fnt.setBold(false);
  p.setFont(fnt);
  p.end();
}
