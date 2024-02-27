#include "commitpad.h"
#include <QApplication>
#include <QSettings>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QString settingsFileLocation = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
  settingsFileLocation += "/commitpad.ini";

  QSettings settings( settingsFileLocation, QSettings::IniFormat );

  CommitPad w( settings );

  if( settings.contains( "WindowPosition" ) )
  {
    w.setGeometry( settings.value( "WindowPosition" ).toRect() );
  }
  w.show();

  int r = a.exec();

  settings.setValue( "WindowPosition", QVariant( w.geometry() ) );

  return r;
}
