#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "prfconst.h"
#include <bps/navigator.h>

#define BBPREF_VERSION "1.0"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->titleLabel->setText(
    ui->titleLabel->text().replace("BBPREF_VERSION", BBPREF_VERSION) );
  connect( ui->titleLabel, SIGNAL(linkActivated(QString)), this, SLOT(onLinkActivated(QString)) );
  showMaximized();
}

AboutDialog::~AboutDialog()
{
  delete ui;
}

void AboutDialog::onLinkActivated(QString i_link)
{
  navigator_invoke(i_link.toAscii().data(), NULL);
}
