#include <QtGui/QVBoxLayout>
#include <QtGui/QTableWidget>
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
    const QList<GameLogEntry> log = model->gameLog();
    QTableWidget *table = new QTableWidget(log.size(), 4, this);
    for (int i=0; i<3; i++)
        table->setHorizontalHeaderItem(i, new QTableWidgetItem(model->player(i+1)->nick()));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem(tr("Time")));
//    qDebug() << log.size();
    for (int i=0; i<log.size(); i++) {
        for (int j=0; j<3; j++)
            table->setItem(i, j, new QTableWidgetItem(QString::number(log.at(i).score[j])));
        table->setItem(i, 3, new QTableWidgetItem(QString::number(log.at(i).time)));
    }
    layout->addWidget(table);
    setLayout(layout);
}
