#ifndef COMMITMESSAGEHISTORYSTRINGLISTMODEL
#define COMMITMESSAGEHISTORYSTRINGLISTMODEL

#include "commitmessagehistorymodelinterface.h"

#include <QStandardItemModel>
 
/**
* Model which stores and retrieves commit messages in a QStringList
**/
class CommitMessageHistoryItemModel : public QStandardItemModel, public CommitHistoryModelInterface
{
public:
  CommitMessageHistoryItemModel( QObject *parent = 0 );
  virtual ~CommitMessageHistoryItemModel();
 
  /// add a single commit message to the model
  void pushCommitMessage( const QString &message ) override;

  /// clear the entire model
  virtual void clear() final override;

  /// return the last `count` first lines of commit messages in the model
  virtual QStringList messageHeads( int count = 10 ) const override;

  /// return the full test of the message at `index` in the model, where the most recent message is index = 0
  virtual QString message( int index ) const override;

private:
  QStandardItem *m_emptyItem;
};


#endif COMMITMESSAGEHISTORYSTRINGLISTMODEL
