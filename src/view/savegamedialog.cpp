#include "savegamedialog.h"
#include "ui_savegamedialog.h"

#include <QMessageBox>

#include "desktop.h"

SaveGameDialog::SaveGameDialog(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::SaveGameDialog)
{
  ui->setupUi(this);
  d_buttons.addButton(ui->btnSlot1, 1);
  d_buttons.addButton(ui->btnSlot2, 2);
  d_buttons.addButton(ui->btnSlot3, 3);
  d_buttons.addButton(ui->btnSlot4, 4);
  d_buttons.addButton(ui->btnSlot5, 5);
  connect(&d_buttons, SIGNAL(buttonClicked(int)), SLOT(buttonClicked(int)));
  setupButtons();
}

SaveGameDialog::~SaveGameDialog()
{
  delete ui;
}

void SaveGameDialog::setupButtons()
{
  d_empty.clear();
  for ( int i = 1; i <= 5; i++ )
  {
    QString caption = PrefModel::getHeader( gameSlotName(i) );
    if (caption.isEmpty())
    {
      caption = tr("Empty");
      d_empty.insert(i);
    }
    else
      d_buttons.button(i)->setText(caption);
  }
}

void SaveGameDialog::buttonClicked(int i_id)
{
  if (!d_empty.contains(i_id))
  {
    int answer = QMessageBox::question(this, tr("Confirmation"),
        tr("Are you sure you want to replace saved game data?"),
        QMessageBox::Yes | QMessageBox::No );
    if (answer == QMessageBox::No)
      return;
  }
  d_selection = gameSlotName(i_id);
  emit accepted();
}
