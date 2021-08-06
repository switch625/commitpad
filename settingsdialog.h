#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog( QSettings &settings, QWidget *parent = 0);
  ~SettingsDialog();

  inline static QString autofillIssueSettingsKey() { return "autofillIssueNumber"; }

public slots:
  void accept();

protected:
  virtual void showEvent(QShowEvent *event);
  void readSettings();
  void writeSettings() const;

private:
  Ui::SettingsDialog *ui;
  QSettings &m_settings;
};

#endif // SETTINGSDIALOG_H
