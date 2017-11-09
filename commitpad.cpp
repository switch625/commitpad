#include "commitpad.h"
#include "ui_commitpad.h"
#include "commitsyntaxhighlighter.h"

#include <QTextStream>
#include <QSignalMapper>

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
    updateToolBar();
  }
}

void CommitPad::updateToolBar()
{
  QRegExp rx(CommitSyntaxHighlighter::jiraIssueKeySearchExpression());

  ui->insertLabel->setVisible( false );
  while( ui->toolBar->actions().count() )
  {
    delete ui->toolBar->actions().first();
  }
  delete ui->toolBar->findChild< QSignalMapper * >();

  int pos = 0;
  QString text( ui->editor->toPlainText() );
  QStringList jiraIssueKeys;
  while( ( pos = rx.indexIn( text, pos ) ) != -1 )
  {
    if( !jiraIssueKeys.contains( rx.capturedTexts().first() ) )
    {
      jiraIssueKeys.append( rx.capturedTexts().first() );
      ui->insertLabel->setVisible( true );
    }
    pos += rx.matchedLength();
  }

  QSignalMapper *keyMapper = new QSignalMapper( ui->toolBar );
  connect( keyMapper, SIGNAL( mapped( QString ) ), SLOT( onInsertJiraKey( QString ) ) );
  for( int keyIndex = 0; keyIndex < jiraIssueKeys.count(); ++keyIndex )
  {
    const QString &jiraIssueKey = jiraIssueKeys.at( keyIndex );

    QString keySequence;
    if( keyIndex < 8 )
      keySequence = QString( "Alt+%1" ).arg( keyIndex + 1 );

    QString buttonText = jiraIssueKey;
    if( !keySequence.isEmpty() )
    {
      buttonText += QString( " (%1)" ).arg( keySequence );
    }

    QAction *keyAction = new QAction( buttonText, ui->toolBar );
    keyAction->setShortcut( QKeySequence( keySequence ) );

    ui->toolBar->addAction( keyAction );
    keyMapper->setMapping( keyAction, jiraIssueKey );
    connect( keyAction, SIGNAL( triggered() ), keyMapper, SLOT( map() ) );
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

void CommitPad::onInsertJiraKey( const QString &key )
{
  ui->editor->insertPlainText( key + ": " );
}
