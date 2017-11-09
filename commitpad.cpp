#include "commitpad.h"
#include "ui_commitpad.h"
#include "commitsyntaxhighlighter.h"

#include <QTextStream>

CommitPad::CommitPad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CommitPad)
{
  ui->setupUi(this);

  setWindowFlags( Qt::Widget | Qt::FramelessWindowHint );

  ui->titleBar->setTitle( "CommitPad" );

  QAction *commitAction = new QAction( QIcon( ":/icons/commit" ), tr( "Commit" ), this );
  commitAction->setShortcut( QKeySequence( "Ctrl+Return" ) );
  ui->commitButton->setDefaultAction( commitAction );

  connect( commitAction, SIGNAL( triggered() ), SLOT( commit() ) );

  QAction *cancelAction = new QAction( QIcon( ":/icons/cancel" ), tr( "Cancel" ), this );
  cancelAction->setShortcut( QKeySequence( "Esc" ) );
  ui->cancelButton->setDefaultAction( cancelAction );

  connect( cancelAction, SIGNAL( triggered() ), SLOT( cancel() ) );

  new CommitSyntaxHighlighter( ui->editor->document() );

  QStringList args( QApplication::instance()->arguments() );
  args.takeFirst();
  foreach( const QString &arg, args )
  {
    if( !arg.isEmpty() )
    {
      if( QFile::exists( arg ) )
      {
        loadFile( arg );
      }
    }
  }
}

CommitPad::~CommitPad()
{
  delete ui;
}

void CommitPad::loadFile( const QString &filename )
{
  m_filename = filename;
  QFile f( filename );
  if( f.open( QIODevice::ReadOnly ) )
  {
    ui->editor->setPlainText( QString( f.readAll() ) );
  }
}

void CommitPad::commit()
{
  QFile f( m_filename );
  if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
  {
    QTextStream s( &f );
    s << ui->editor->toPlainText();
    f.close();
    close();
  }
  else
  {
    emit warningMsg( tr( "Could not open file for writing" ) );
  }
}

void CommitPad::cancel()
{
  QFile f( m_filename );
  if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
  {
    f.close();
    close();
  }
  else
  {
    emit warningMsg( tr( "Could not open file for writing" ) );
  }
}
