
#ifndef NEWGAMEFORM_H
#define NEWGAMEFORM_H

#include <QDialog>


#include "ui_newgameform.h"
class NewGameDialog : public QDialog, public Ui_NewGameDialog {
  Q_OBJECT

public:
  NewGameDialog (QWidget *parent=0);
  ~NewGameDialog () { }

protected:
  void showEvent (QShowEvent *event);
};


#endif
