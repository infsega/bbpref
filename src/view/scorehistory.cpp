#include <QtGui/QVBoxLayout>
#include <QtGui/QHeaderView>
//#include <QDebug>

#include "scorehistory.h"
#include "desktop.h"
#include "player.h"

ScoreHistoryDialog::ScoreHistoryDialog(PrefModel *model, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f)
{
    setWindowTitle(tr("Score History"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    const QList<GameLogEntry> log = model->gameLog();
    m_table = new QTableWidget(log.size(), 5, this);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    for (int i=0; i<3; i++)
        m_table->setHorizontalHeaderItem(i, new QTableWidgetItem(model->player(i+1)->nick()));
    m_table->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Time")));
    m_table->setHorizontalHeaderItem(4, new QTableWidgetItem(tr("Deal")));
    for (int i=0; i<log.size(); i++) {
        for (int j=0; j<3; j++)
            m_table->setItem(i, j, new QTableWidgetItem(QString::number(log.at(i).score[j])));
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(log.at(i).time)));
    }
    layout->addWidget(m_table);    
    setLayout(layout);    
}

void ScoreHistoryDialog::showEvent(QShowEvent *event)
{
  Q_UNUSED(event)
  m_table->resizeColumnsToContents();
  m_table->horizontalHeader()->setStretchLastSection(true);
  QSize dialogSize = size();
  dialogSize.setWidth(m_table->horizontalHeader()->length()+m_table->verticalHeader()->width()+10);
  resize(dialogSize);
  //qDebug() << m_table->horizontalHeader()->length();
}
