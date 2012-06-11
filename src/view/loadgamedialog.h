#pragma once

#include <QDialog>
#include <QButtonGroup>
#include <QString>

namespace Ui
{
  class LoadGameDialog;
}

class LoadGameDialog: public QDialog
{
  Q_OBJECT
    
public:
  explicit LoadGameDialog(QWidget *parent = 0);
  ~LoadGameDialog();
    
  QString selection() const { return d_selection; }

private:
  void setupButtons();

private slots:
  void buttonClicked(int i_id);
  void autosaveClicked();

private:
  Ui::LoadGameDialog *ui;
  QButtonGroup d_buttons;
  QString      d_selection;
};
