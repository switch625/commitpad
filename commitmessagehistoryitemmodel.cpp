#include "commitmessagehistoryitemmodel.h"

CommitMessageHistoryItemModel::CommitMessageHistoryItemModel( QObject *parent )
  : QStandardItemModel( parent )
  , CommitHistoryModelInterface()
{
  clear();

  pushCommitMessage( "CIP-4334: First commit message" );
  pushCommitMessage( "CIP-4334: Second commit message" );
  pushCommitMessage( "CIP-4334: Third commit message" );
  pushCommitMessage( "CIP-4344: Latest commit message" );
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
  for( int rowIndex = 1; rowIndex < count && rowIndex > rowCount(); ++rowIndex )
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

