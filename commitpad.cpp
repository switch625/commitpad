#include "commitpad.h"
#include "ui_commitpad.h"
#include "commitsyntaxhighlighter.h"
#include "settingsdialog.h"
#include "commitmessagehistoryitemmodel.h"

#include <QTextStream>
#include <QSignalMapper>
#include <QFileInfo>
#include <QMessageBox>
#include <QShowEvent>
#include <QWinTaskbarButton>
#include <QStyleFactory>

CommitPad::CommitPad( QSettings &settings, QWidget *parent )
  : QMainWindow(parent)
  , m_settings( settings )
  , ui(new Ui::CommitPad)
{
  setApplicationPalette();
  ui->setupUi(this);
  ui->insertLabel->setVisible( false );

  m_result = Undefined;

  m_operationIcons.insert( "COMMIT_EDITMSG", generateActionIcon( ":/operations/git-commit" ) );
  m_operationIcons.insert( "git-rebase-todo", generateActionIcon( ":/operations/git-branch" ) );
  m_operationIcons.insert( "MERGE_MSG", generateActionIcon( ":/operations/git-merge" ) );

  setWindowFlags( Qt::Widget | Qt::FramelessWindowHint );

  ui->titleBar->setTitle( "CommitPad" );
  setWindowIcon( QIcon( ":/icons/icon" ) );

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

  CommitMessageHistoryItemModel *historyModel = new CommitMessageHistoryItemModel( this );
  ui->historyCombo->setModel( historyModel );
  m_history = historyModel;

  connect( ui->historyCombo, SIGNAL( activated( int ) ), SLOT( onComboIndexSelected( int ) ) );
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
  delete ui->toolBar->findChild< QSignalMapper* >();

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

    QString filename( QFileInfo( m_filename ).fileName() );
    if( m_operationIcons.contains( filename ) )
    {
      QWinTaskbarButton *button = new QWinTaskbarButton( this );
      button->setWindow( windowHandle() );
      button->setOverlayIcon( m_operationIcons.value( filename ) );
    }
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

  if( m_filename.isEmpty() || ( !m_operationIcons.uniqueKeys().contains( QFileInfo( m_filename ).fileName() ) && m_result == Reject ) )
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

        if( m_filename == "COMMIT_EDITMSG" )
        {
          QString commitMessage( ui->editor->toPlainText() );
          commitMessage = commitMessage.left( commitMessage.indexOf( "#" ) ).trimmed(); // strip git comments and trailing whitespace
          m_history->pushCommitMessage( commitMessage );
        }
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


QIcon CommitPad::generateActionIcon( const QString &iconFilename )
{
  return QIcon( iconFilename );
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

void CommitPad::onComboIndexSelected( int comboIndex )
{
  // replace the message in the editor with that in the history model and reset the combo to be blank.
  QString commitMessage = m_history->message( comboIndex );

  ui->editor->setPlainText( commitMessage );
  ui->historyCombo->setCurrentIndex( 0 );
}

void CommitPad::setApplicationPalette() const
{
#ifdef Q_OS_WIN
  QSettings settings( "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat );
  if( settings.value( "AppsUseLightTheme" ) == 0 ) 
  {
    qApp->setStyle( QStyleFactory::create( "Fusion" ) );
    QPalette darkPalette;
    QColor darkColor = QColor( 45, 45, 45 );
    QColor disabledColor = QColor( 127, 127, 127 );
    darkPalette.setColor( QPalette::Window, darkColor );
    darkPalette.setColor( QPalette::WindowText, Qt::white );
    darkPalette.setColor( QPalette::Base, QColor( 18, 18, 18 ) );
    darkPalette.setColor( QPalette::AlternateBase, darkColor );
    darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
    darkPalette.setColor( QPalette::ToolTipText, Qt::white );
    darkPalette.setColor( QPalette::Text, Qt::white );
    darkPalette.setColor( QPalette::Disabled, QPalette::Text, disabledColor );
    darkPalette.setColor( QPalette::Button, darkColor );
    darkPalette.setColor( QPalette::ButtonText, Qt::white );
    darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, disabledColor );
    darkPalette.setColor( QPalette::BrightText, Qt::red );
    darkPalette.setColor( QPalette::Link, QColor( 42, 130, 218 ) );

    darkPalette.setColor( QPalette::Highlight, QColor( 42, 130, 218 ) );
    darkPalette.setColor( QPalette::HighlightedText, Qt::black );
    darkPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, disabledColor );

    qApp->setPalette( darkPalette );

    qApp->setStyleSheet( "QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }" );
  }
#endif
}
