#include "commitpad.h"
#include <QApplication>
#include <QSettings>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QSettings settings( "CommitPad" );

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
