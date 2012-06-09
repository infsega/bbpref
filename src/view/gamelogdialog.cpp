#include "gamelogdialog.h"
#include "ui_gamelogdialog.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QMap>

#include "desktop.h"
#include "player.h"

DealButton::DealButton(int rowNum, const QString& text, QWidget* parent)
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
  QStandardItemModel* treeModel = new QStandardItemModel(this);
  ui->tableView->setModel(treeModel);
  QStringList headers;
  headers.append(tr("Game"));
  headers.append(model->player(1)->nick());
  headers.append(model->player(2)->nick());
  headers.append(model->player(3)->nick());
  headers.append(tr("Time"));
  headers.append(tr("Deal"));
  treeModel->setHorizontalHeaderLabels(headers);
  for (int i = 0; i < model->gameLog().size(); i++)
  {
    GameLogEntry entry = model->gameLog().at(i);
    QList<QStandardItem*> items;

    QString gameTitle = sGameName(entry.game);
    gameTitle.replace("\1s", QChar((ushort)0x2660));
    gameTitle.replace("\1c", QChar((ushort)0x2663));
    gameTitle.replace("\1d", QChar((ushort)0x2666));
    gameTitle.replace("\1h", QChar((ushort)0x2665));
    items.append( new QStandardItem(gameTitle.trimmed()) );

    QMap<int, QString> playerResult;
    for ( int player = 1; player <= 3; player++ )
    {
      QString& res = playerResult[player];
      res = QString::number(entry.takes[player - 1]);
      if (player == entry.player)
        res += "*";
      if (entry.whist[player - 1])
        res += tr("(w)");
      items.append( new QStandardItem(res) );
    }

    QString time = QString::number(entry.time, 'f', 1);
    items.append( new QStandardItem(time) );
    QString dummy = tr("Show");
    items.append( new QStandardItem(dummy) );

    treeModel->appendRow(items);

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
    CardList cl;
    foreach(const Card& c, model->gameLog().at(row).cardList[i-1])
      cl.insert(const_cast<Card*>(&c));
    cl.mySort();
    for( int ci = 0; ci < cl.count(); ci++ )
    {
      output += cl.at(ci)->toUniString();
      output += ' ';
    }
    output += '\n';
  }
  QMessageBox::about(0, tr("Deal #") + QString::number(row + 1), output);
}
