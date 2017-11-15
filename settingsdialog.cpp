#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QShowEvent>

SettingsDialog::SettingsDialog(QSettings &settings, QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , m_settings(settings)
{
  ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}

void SettingsDialog::accept()
{
  writeSettings();
  QDialog::accept();
}

void SettingsDialog::showEvent(QShowEvent *event)
{
  if( !event->spontaneous() )
    readSettings();
}

void SettingsDialog::readSettings()
{
  ui->autofillIssueNumberCheckBox->setChecked( m_settings.value( "autofillIssueNumber" ).toBool() );
}

void SettingsDialog::writeSettings() const
{
  if( m_settings.value( "autofillIssueNumber" ).toBool() != ui->autofillIssueNumberCheckBox->isChecked() )
  {
    QVariant v;
    v.setValue< bool >( ui->autofillIssueNumberCheckBox->isChecked() );
    m_settings.setValue( "autofillIssueNumber", v );
  }
}
