#pragma once

#include <QDialog>
#include <QPushButton>

namespace Ui
{
  class GameLogDialog;
}

class PrefModel;

class GameLogDialog : public QDialog
{
  Q_OBJECT
    
public:
  explicit GameLogDialog(PrefModel* i_model, QWidget *parent = 0);
  ~GameLogDialog();

protected:
  void showEvent(QShowEvent *event);

private slots:
  void showDeal(const int n);

private:
  Ui::GameLogDialog *ui;
  PrefModel* model;
};

class DealButton : public QPushButton
{
  Q_OBJECT

public:
  explicit DealButton(int rowNum, const QString& text, QWidget* parent = 0);

signals:
  void clicked(int);

private slots:
  void doClick() { emit clicked(m_rowNum); }

private:
  int m_rowNum;
};

