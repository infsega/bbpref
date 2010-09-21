#include <QtCore/QAbstractTableModel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QPainter>
#include <QtGui/QMessageBox>
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
      return 6;
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
          {
          QString s(sGameName(m_log.at(index.row()).game));
//          if (s.indexOf("\1d") >= 0 || s.indexOf("\1h") >= 0) textRGB = qRgb(255, 0, 0);
          s.replace("\1s", QChar((ushort)0x2660));
          s.replace("\1c", QChar((ushort)0x2663));
          s.replace("\1d", QChar((ushort)0x2666));
          s.replace("\1h", QChar((ushort)0x2665));
          s = s.trimmed();
          return s;
          }
        case 1:
        case 2:
        case 3:
            return QString::number(m_log.at(index.row()).takes[index.column()-1])
                + (index.column() == m_log.at(index.row()).player ? "*" : "")
                + (m_log.at(index.row()).whist[index.column()-1] ? tr("(w)") : "");
        case 4:
          return QString::number(m_log.at(index.row()).time, 'f', 1);
        case 5:
          return tr("Show");
        }
      } else {
        return QVariant();
      }
    }

    Qt::ItemFlags flags(const QModelIndex &index) const
    {
      Q_UNUSED(index)
      return Qt::ItemIsEnabled;
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
          return tr("Game");
        case 1:
        case 2:
        case 3:
          return m_model->player(section)->nick();
        case 4:
          return tr("Time");
        case 5:
          return tr("Deal");
        }
      } else {
        return QVariant();
      }
    }
  };
}


DealButton::DealButton(int rowNum, const QString & text, QWidget * parent)
  : m_rowNum(rowNum), QPushButton(text, parent)
{
  connect(this, SIGNAL(clicked()), this, SLOT(doClick()));
}


ScoreHistoryDialog::ScoreHistoryDialog(PrefModel *model, QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f), m_model(model)
{
    setWindowTitle(tr("Score History"));
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(5);
    layout->setContentsMargins(0,0,0,0);
    m_table = new QTableView(this);
    m_table->setAlternatingRowColors(true);
    m_table->setModel(new GameLogModel(model, this));
    for (int i=0; i<model->gameLog().size(); i++) {
      DealButton *btnDeal = new DealButton(i, tr("Deal #") + QString::number(i+1), m_table);
      m_table->setIndexWidget(m_table->model()->index(i, 5), btnDeal);
      connect(btnDeal, SIGNAL(clicked(int)), this, SLOT(showDeal(int)));
    }
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

void ScoreHistoryDialog::showDeal(const int row)
{
  Q_ASSERT(row < m_model->gameLog().size());
  QString output;
  for (int i=1; i<=3; i++) {
    output += QString("%1").arg(m_model->player(i)->nick(), -12);
    output += ": ";
    foreach(const Card &c, m_model->gameLog().at(row).cardList[i-1]) {
      output += c.toUniString();
      output += ' ';
    }
    output += '\n';
  }
  QMessageBox::about(0, tr("Deal #") + QString::number(row + 1), output);
}
