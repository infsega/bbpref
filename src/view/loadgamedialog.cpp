#include "loadgamedialog.h"
#include "ui_loadgamedialog.h"

#include <QDebug>

#include "desktop.h"

LoadGameDialog::LoadGameDialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::LoadGameDialog)
{
  ui->setupUi(this);
  d_buttons.addButton(ui->btnSlot1, 1);
  d_buttons.addButton(ui->btnSlot2, 2);
  d_buttons.addButton(ui->btnSlot3, 3);
  d_buttons.addButton(ui->btnSlot4, 4);
  d_buttons.addButton(ui->btnSlot5, 5);
  connect(&d_buttons, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));
  connect(ui->btnAutoSave, SIGNAL(clicked()), SLOT(autosaveClicked()));
  setupButtons();
}

LoadGameDialog::~LoadGameDialog()
{
  delete ui;
}

void LoadGameDialog::setupButtons()
{
  QString autoSaveCaption = PrefModel::getHeader( autoSaveSlotName() );
  if (autoSaveCaption.isEmpty())
  {
    ui->btnAutoSave->setText(tr("Empty Slot"));
    ui->btnAutoSave->setEnabled(false);
  }
  else
    ui->btnAutoSave->setText(tr("[autosave]") + " " + autoSaveCaption);

  for ( int i = 1; i <= 5; i++ )
  {
    QString caption = PrefModel::getHeader( gameSlotName(i) );
    if (caption.isEmpty())
    {
      caption = tr("Empty Slot");
      d_buttons.button(i)->setEnabled(false);
    }
    d_buttons.button(i)->setText(caption);
  }
}

void LoadGameDialog::buttonClicked(int i_id)
{
  d_selection = gameSlotName(i_id);
  qDebug() << d_selection << " chosen for load";
  accept();
}

void LoadGameDialog::autosaveClicked()
{
  d_selection = autoSaveSlotName();
  qDebug() << d_selection << " chosen for save";
  accept();
}
