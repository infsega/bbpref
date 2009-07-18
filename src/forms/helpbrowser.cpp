
#include <QDesktopWidget>
#include <QDialog>

#include "helpbrowser.h"

HelpBrowser::HelpBrowser (QWidget *parent) : QDialog(parent) {
  setupUi(this);
  setAttribute(Qt::WA_QuitOnClose, false);
  setAttribute(Qt::WA_DeleteOnClose, false);
}


void HelpBrowser::showEvent (QShowEvent *event) {
  Q_UNUSED(event)
  QWidget *parentObj = dynamic_cast<QWidget *>(parent());
  int x, y;
  if (parentObj) {
    QRect dims(parentObj->frameGeometry());
    x = dims.left()+(dims.width()-width())/2;
    y = dims.top()+(dims.height()-height())/2;
  } else {
    QDesktopWidget *desk = QApplication::desktop();
    QRect dims(desk->availableGeometry(this));
    x = dims.left()+(dims.width()-width())/2;
    y = dims.top()+(dims.height()-height())/2;
  }
  move(x, y);
}
