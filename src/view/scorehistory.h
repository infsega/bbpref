#ifndef SCOREHISTORY_H
#define SCOREHISTORY_H

#include <QtGui/QDialog>
#include <QtGui/QTableView>
#include <QtGui/QPushButton>

class PrefModel;
class ScoreHistoryDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ScoreHistoryDialog(PrefModel *model, QWidget *parent = 0, Qt::WindowFlags f = 0);

protected:
  void showEvent(QShowEvent *event);

private slots:
  void showDeal(const int n);

private:
  QTableView *m_table;
  PrefModel *m_model;
};

class DealButton : public QPushButton
{
  Q_OBJECT

public:
  explicit DealButton(int rowNum, const QString & text, QWidget * parent = 0);

signals:
  void clicked(int);

private slots:
  void doClick() { emit clicked(m_rowNum); }

private:
  int m_rowNum;
};

#endif // SCOREHISTORY_H
