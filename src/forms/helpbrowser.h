
#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QDialog>


#include "ui_helpbrowser.h"
class HelpBrowser : public QDialog, public Ui_HelpBrowser {
  Q_OBJECT

public:
  HelpBrowser (QWidget *parent=0);
  ~HelpBrowser () { }

protected:
  void showEvent (QShowEvent *event);
};


#endif
