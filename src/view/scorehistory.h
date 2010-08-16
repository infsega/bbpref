#ifndef SCOREHISTORY_H
#define SCOREHISTORY_H

#include <QtGui/QDialog>
#include <QtGui/QTableWidget>

class PrefModel;
class ScoreHistoryDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ScoreHistoryDialog(PrefModel *model, QWidget *parent = 0, Qt::WindowFlags f = 0);

protected:
  void showEvent(QShowEvent *event);

private:
  QTableWidget *m_table;
};

#endif // SCOREHISTORY_H
