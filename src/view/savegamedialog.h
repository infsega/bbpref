#pragma once

#include <QDialog>
#include <QButtonGroup>
#include <QSet>
#include <QString>

namespace Ui
{
  class SaveGameDialog;
}

class SaveGameDialog: public QDialog
{
  Q_OBJECT
    
public:
  explicit SaveGameDialog(QWidget *parent = 0);
  ~SaveGameDialog();

  QString selection() const { return d_selection; }

private:
  void setupButtons();

private slots:
  void buttonClicked(int i_id);

private:
  Ui::SaveGameDialog *ui;
  QButtonGroup d_buttons;
  QString      d_selection;
  QSet<int>    d_empty;
};
