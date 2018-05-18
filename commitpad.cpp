#include "commitpad.h"
#include "ui_commitpad.h"
#include "commitsyntaxhighlighter.h"
#include "settingsdialog.h"

#include <QTextStream>
#include <QSignalMapper>
#include <QFileInfo>
#include <QMessageBox>
#include <QShowEvent>

CommitPad::CommitPad( QSettings &settings, QWidget *parent )
  : QMainWindow(parent)
  , m_settings( settings )
  , ui(new Ui::CommitPad)
{
  ui->setupUi(this);
  ui->insertLabel->setVisible( false );

  m_result = Undefined;

  m_commitMessageFilenames << "COMMIT_EDITMSG" << "git-rebase-todo";

  setWindowFlags( Qt::Widget | Qt::FramelessWindowHint );

  ui->titleBar->setTitle( "CommitPad" );

  QAction *commitAction = new QAction( QIcon( ":/icons/commit" ), tr( "Commit" ), this );
  commitAction->setShortcut( QKeySequence( "Ctrl+Return" ) );
  ui->commitButton->setDefaultAction( commitAction );

  connect( commitAction, SIGNAL( triggered() ), SLOT( onCommit() ) );

  QAction *cancelAction = new QAction( QIcon( ":/icons/cancel" ), tr( "Cancel" ), this );
  cancelAction->setShortcut( QKeySequence( "Esc" ) );
  ui->cancelButton->setDefaultAction( cancelAction );

  connect( cancelAction, SIGNAL( triggered() ), SLOT( onCancel() ) );

  QAction *settingsAction = new QAction( QIcon( ":/icons/settings" ), tr( "Settings" ), this );
  ui->settingsButton->setDefaultAction( settingsAction);
  connect( settingsAction, SIGNAL( triggered() ), this, SLOT( onSettingsButtonClicked() ) );

  connect( this, SIGNAL( warningMsg( QString ) ), ui->statusBar, SLOT( showMessage( QString ) ) );
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
        ui->titleBar->setTitle( QString( "%1 - CommitPad" ).arg( QFileInfo( arg ).fileName() ) );
        break;
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

  const int maxIssueKeys = 5; // this is the maximum number of issue keys we will display on the toolbar
  int keyCount = 0;
  int pos = 0;
  QString text( ui->editor->toPlainText() );
  QStringList jiraIssueKeys;
  while( ( pos = rx.indexIn( text, pos ) ) != -1 && keyCount < maxIssueKeys )
  {
    if( !jiraIssueKeys.contains( rx.capturedTexts().first() ) )
    {
      jiraIssueKeys.append( rx.capturedTexts().first() );
      ui->insertLabel->setVisible( true );
    }
    pos += rx.matchedLength();
    ++keyCount;
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

  if( !jiraIssueKeys.isEmpty() && m_settings.value( SettingsDialog::autofillIssueSettingsKey() ).toBool() == true )
  {
    if( ui->editor->toPlainText().isEmpty() || ui->editor->toPlainText().startsWith( "\n" ) )
    {
      QString insertText = jiraIssueKeys.first() + ": ";
      ui->editor->insertPlainText( insertText );
    }
  }
}

void CommitPad::showEvent( QShowEvent *event )
{
  if( !event->spontaneous() )
  {
    activateWindow();
    ui->editor->setFocus();
  }
}

void CommitPad::closeEvent( QCloseEvent *event )
{
  if( m_result == Undefined && !m_filename.isEmpty() )
  {
    QMessageBox::StandardButton response = QMessageBox::question( this, "CommitPad", tr( "Commit?" ), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Cancel );
    switch( response )
    {
    case QMessageBox::Yes:
      m_result = Accept;
      break;
    case QMessageBox::No:
      m_result = Reject;
      break;
    case QMessageBox::Cancel:
    default:
      event->ignore();
      return;
    }
  }

  if( m_filename.isEmpty() || ( !m_commitMessageFilenames.contains( QFileInfo( m_filename ).fileName() ) && m_result == Reject ) )
  {
    // just close
    event->accept();
  }
  else
  {
    QFile f( m_filename );
    if( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
      if( m_result == Accept )
      {
        QTextStream s( &f );
        s << ui->editor->toPlainText();
      }
      f.close();
      event->accept();
    }
    else
    {
      emit warningMsg( tr( "Could not open file for writing" ) );
      event->ignore();
    }
  }
}

void CommitPad::onCommit()
{
  m_result = Accept;
  close();
}

void CommitPad::onCancel()
{
  m_result = Reject;
  close();
}

void CommitPad::onSettingsButtonClicked()
{
  SettingsDialog dlg( m_settings );
  dlg.exec();
}

void CommitPad::onInsertJiraKey( const QString &key )
{
  ui->editor->insertPlainText( key + ": " );
}
