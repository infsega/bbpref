#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <bps/navigator.h>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->titleLabel->text().replace("BBPREF_VERSION", BBPREF_VERSION);
  connect( ui->titleLabel, SIGNAL(linkActivated(QString)), this, SLOT(onLinkActivated(QString)) );
  connect( ui->buttonBox,  SIGNAL(accepted()), this, SLOT(accept()) );
  showFullScreen();
}

AboutDialog::~AboutDialog()
{
  delete ui;
}

void AboutDialog::onLinkActivated(QString i_link)
{
  navigator_invoke(i_link.toAscii().data(), NULL);
}
