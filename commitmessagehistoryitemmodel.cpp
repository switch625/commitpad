#include "commitmessagehistoryitemmodel.h"

#include <QSettings>

CommitMessageHistoryItemModel::CommitMessageHistoryItemModel( QObject *parent )
  : QStandardItemModel( parent )
  , CommitHistoryModelInterface()
{
  clear();
}

CommitMessageHistoryItemModel::~CommitMessageHistoryItemModel()
{

}


void CommitMessageHistoryItemModel::pushCommitMessage( const QString &message )
{
  QStringList lines( message.split( "\n" ) );
  if( !lines.isEmpty() )
  {
    QStandardItem *item = new QStandardItem( lines.first() );
    item->setData( QVariant( message ), Qt::UserRole );
    insertRow( 1, item );
  }
}

void CommitMessageHistoryItemModel::clear()
{
  QStandardItemModel::clear();

  m_emptyItem = new QStandardItem( QString() );
  appendRow( m_emptyItem );
}

QStringList CommitMessageHistoryItemModel::messageHeads( int count ) const
{
  QStringList heads;

  // ignore row 0
  for( int rowIndex = 1; rowIndex < count && rowIndex < rowCount(); ++rowIndex )
  {
    heads << item( rowIndex )->text();
  }

  return heads;
}

QString CommitMessageHistoryItemModel::message( int index ) const
{
  QString message;

  if( index < rowCount() )
  {
    message = item( index )->data( Qt::UserRole ).toString();
  }

  return message;
}

void CommitMessageHistoryItemModel::load( QSettings &settings )
{
  clear();

  m_settings = &settings;
  int historySize = settings.beginReadArray( "history" );
  for( int historyIndex = 0; historyIndex < historySize && historyIndex < modelCountLimit; ++historyIndex )
  {
    settings.setArrayIndex( historyIndex );
    QStandardItem *item = new QStandardItem( settings.value( "message" ).toString() );
    appendRow( item );
  }
  settings.endArray();
}

void CommitMessageHistoryItemModel::save()
{
  if( !m_settings.isNull() )
  {
    m_settings->beginWriteArray( "history" );
    for( int rowIndex = 1; rowIndex <= modelCountLimit && rowIndex < rowCount(); ++rowIndex )
    {
      m_settings->setArrayIndex( rowIndex - 1 );
      m_settings->setValue( "message", item( rowIndex )->data( Qt::UserRole ).toString() );
    }
    m_settings->endArray();
  }
}

