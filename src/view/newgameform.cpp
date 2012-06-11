#include "newgameform.h"
#include "ui_newgameform.h"

#include <QSettings>

#include <bps/virtualkeyboard.h>

#include "desktop.h"

NewGameDialog::NewGameDialog(PrefModel* i_model, QWidget *parent /* = NULL */)
  : QDialog(parent)
  , ui( new Ui::NewGameDialog() )
  , model(i_model)
{
  ui->setupUi(this);
  connect(ui->cbRounds, SIGNAL(stateChanged(int)), SLOT(toggleRounds(int)));

  QSettings st;
  // Players
  ui->leHumanName->setText(model->optHumanName);
  ui->leName1->setText(st.value("playername1", tr("Mr. West")).toString());
  ui->leName2->setText(st.value("playername2", tr("Dr. East")).toString());
  ui->cbAlphaBeta1->setChecked(st.value("alphabeta1", true).toBool());
  ui->cbAlphaBeta2->setChecked(st.value("alphabeta2", true).toBool());

  // Conventions
  ui->sbGame->setValue(st.value("maxpool", 10).toInt());
  if(st.value("quitmaxrounds", false).toBool())
  {
    ui->cbRounds->setCheckState(Qt::Checked);
    ui->sbRounds->setValue(st.value("maxrounds", -1).toInt());
  }
  ui->cbGreedy->setChecked(st.value("whistgreedy", true).toBool());
  ui->cbTenWhist->setChecked(st.value("whist10", false).toBool());
  ui->cbStalin->setChecked(st.value("stalin", false).toBool());
  ui->cbAggPass->setChecked(st.value("aggpass", false).toBool());
  ui->cbWithoutThree->setChecked(st.value("without3", false).toBool());
}

NewGameDialog::~NewGameDialog()
{
  delete ui;
}

void NewGameDialog::showEvent(QShowEvent *)
{
  showFullScreen();
}

void NewGameDialog::toggleRounds(int checked)
{
  ui->sbRounds->setEnabled(checked == Qt::Checked);
}

void NewGameDialog::copyOptions(PrefModel& o_model)
{
  // Conventions
  o_model.optMaxPool      = ui->sbGame->value();
  o_model.optQuitAfterMaxRounds = (ui->cbRounds->checkState() == Qt::Checked);
  if (o_model.optQuitAfterMaxRounds)
    o_model.optMaxRounds  = ui->sbRounds->value();
  o_model.optWhistGreedy  = ui->cbGreedy->isChecked();
  o_model.opt10Whist      = ui->cbTenWhist->isChecked();
  o_model.optStalingrad   = ui->cbStalin->isChecked();
  o_model.optAggPass      = ui->cbAggPass->isChecked();
  o_model.optWithoutThree = ui->cbWithoutThree->isChecked();
  o_model.optPassCount    = 0;

  // Players
  o_model.optHumanName    = ui->leHumanName->text();
  o_model.optPlayerName1  = ui->leName1->text();
  o_model.optAlphaBeta1   = ui->cbAlphaBeta1->isChecked();
  o_model.optPlayerName2  = ui->leName2->text();
  o_model.optAlphaBeta2   = ui->cbAlphaBeta2->isChecked();
}

void NewGameDialog::on_buttonBox_accepted()
{
  virtualkeyboard_hide();
}
