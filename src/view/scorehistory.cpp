#include <QtCore/QAbstractTableModel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QDebug>

#include "scorehistory.h"
#include "desktop.h"
#include "player.h"

namespace {
  class GameLogModel : public QAbstractTableModel
  {
  private:
    QList<GameLogEntry> m_log;
    PrefModel *m_model;

  public:
    explicit GameLogModel(PrefModel *model, QObject * parent = 0) : QAbstractTableModel(parent),
                                                           m_model(model)
    {
      m_log = model->gameLog();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent)
      return m_log.size();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const
    {
      Q_UNUSED(parent)
      return 5;
    }

    QVariant data(const QModelIndex &index, int role) const
    {
      if(!index.isValid()) {
        return QVariant();
      }
      if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
      } else if (role == Qt::DisplayRole) {
        switch(index.column()) {
        case 0:
        case 1:
        case 2:
          return QString::number(m_log.at(index.row()).score[index.column()]);
        case 3:
          return QString::number(m_log.at(index.row()).time, 'f', 1);
        case 4:
          return tr("Show");
        }
      } else {
        return QVariant();
      }
    }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
      Q_UNUSED(index)
      return Qt::ItemIsEnabled;//QAbstractItemModel::flags(index);
    }

    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const
    {
      // handle text alignments
      if (role == Qt::TextAlignmentRole) {
        if (orientation == Qt::Vertical) {
          return Qt::AlignHCenter; // XYZ coordinates
        }
      }

      if (role != Qt::DisplayRole)
        return QVariant();

      if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
        case 1:
        case 2:
          return m_model->player(section+1)->nick();
        case 3:
          return tr("Time");
        case 4:
          return tr("Deal");
        }
      } else {
        return QVariant();
      }
    }
  };
}

ScoreHistoryDialog::ScoreHistoryDialog(PrefModel *model, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f), m_model(model)
{
    setWindowTitle(tr("Score History"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    m_table = new QTableView(this);
    m_table->setModel(new GameLogModel(model, this));
    connect(m_table, SIGNAL(clicked(QModelIndex)), this, SLOT(showDeal(QModelIndex)));
    layout->addWidget(m_table);
    setLayout(layout);
}

void ScoreHistoryDialog::showEvent(QShowEvent *event)
{
  Q_UNUSED(event)
  m_table->verticalHeader()->hide();
  m_table->resizeColumnsToContents();
  m_table->horizontalHeader()->setStretchLastSection(true);
  QSize dialogSize = size();
  dialogSize.setWidth(m_table->horizontalHeader()->length()+m_table->verticalHeader()->width()+10);
  resize(dialogSize);
}

void ScoreHistoryDialog::showDeal(const QModelIndex &index)
{
  foreach(const Card &c, m_model->gameLog().at(index.row()).cardList[0])
    qDebug() << c.toString();
}
