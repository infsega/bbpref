#include "gamelogdialog.h"
#include "ui_gamelogdialog.h"

#include <QAbstractTableModel>
#include <QList>
#include <QMessageBox>

#include "desktop.h"
#include "player.h"

class GameLogModel : public QAbstractTableModel
{
private:
  QList<GameLogEntry> m_log;
  PrefModel *m_model;

public:
  explicit GameLogModel(PrefModel *model, QObject * parent = 0)
  : QAbstractTableModel(parent)
  , m_model(model)
  {
    setObjectName("GameLogModel");
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
    if(!index.isValid())
      return QVariant();
    if (role == Qt::TextAlignmentRole)
      return Qt::AlignCenter;
    if (role == Qt::DisplayRole)
    {
      switch(index.column())
      {
      case 0:
        {
          QString s(sGameName(m_log.at(index.row()).game));
          s.replace("\1s", QChar((ushort)0x2660));
          s.replace("\1c", QChar((ushort)0x2663));
          s.replace("\1d", QChar((ushort)0x2666));
          s.replace("\1h", QChar((ushort)0x2665));
          s = s.trimmed();
          return tr(s.toAscii());
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
    }
    return QVariant();
  }

  Qt::ItemFlags flags(const QModelIndex &index) const
  {
    Q_UNUSED(index)
    return Qt::ItemIsEnabled;
  }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
  {
    // handle text alignments
    if (role == Qt::TextAlignmentRole)
      if (orientation == Qt::Vertical)
        return Qt::AlignHCenter; // XYZ coordinates

    if (role != Qt::DisplayRole)
      return QVariant();

    if (orientation == Qt::Horizontal)
    {
      switch (section)
      {
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
    }

    return QVariant();
  }
};

DealButton::DealButton(int rowNum, const QString & text, QWidget * parent)
: QPushButton(text, parent)
, m_rowNum(rowNum)
{
  connect(this, SIGNAL(clicked()), this, SLOT(doClick()));
}

GameLogDialog::GameLogDialog(PrefModel* i_model, QWidget *parent)
: QDialog(parent)
, ui(new Ui::GameLogDialog)
, model(i_model)
{
  ui->setupUi(this);
  ui->tableView->setModel(new GameLogModel(model, this));
  for (int i = 0; i < model->gameLog().size(); i++)
  {
    DealButton *btnDeal = new DealButton(i, tr("Deal #") + QString::number(i + 1), ui->tableView);
    ui->tableView->setIndexWidget(ui->tableView->model()->index(i, 5), btnDeal);
    connect(btnDeal, SIGNAL(clicked(int)), this, SLOT(showDeal(int)));
  }
  showFullScreen();
}

GameLogDialog::~GameLogDialog()
{
  delete ui;
}

void GameLogDialog::showEvent(QShowEvent *event)
{
  Q_UNUSED(event)
  ui->tableView->verticalHeader()->hide();
  ui->tableView->resizeColumnsToContents();
  ui->tableView->horizontalHeader()->setStretchLastSection(true);
}

void GameLogDialog::showDeal(const int row)
{
  Q_ASSERT(row < model->gameLog().size());
  QString output;
  for (int i=1; i<=3; i++)
  {
    output += QString("%1: ").arg(model->player(i)->nick());
    foreach(const Card &c, model->gameLog().at(row).cardList[i-1])
    {
      output += c.toUniString();
      output += ' ';
    }
    output += '\n';
  }
  QMessageBox::about(0, tr("Deal #") + QString::number(row + 1), output);
}
