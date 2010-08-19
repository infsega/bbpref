#ifndef SCOREHISTORY_H
#define SCOREHISTORY_H

#include <QtGui/QDialog>
#include <QtGui/QTableView>

class PrefModel;
class ScoreHistoryDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ScoreHistoryDialog(PrefModel *model, QWidget *parent = 0, Qt::WindowFlags f = 0);

protected:
  void showEvent(QShowEvent *event);

private slots:
  void showDeal(const QModelIndex &index);

private:
  QTableView *m_table;
  PrefModel *m_model;
};

#endif // SCOREHISTORY_H
