#ifndef COMMITMESSAGEHISTORYMODELINTERFACE
#define COMMITMESSAGEHISTORYMODELINTERFACE

#include <QString>

/**
* Interface for a model which stores and retrieves commit messages
**/
class CommitHistoryModelInterface
{
public:
  CommitHistoryModelInterface() {};
  virtual ~CommitHistoryModelInterface() {};

  /// add a single commit message to the model
  virtual void pushCommitMessage( const QString &message ) = 0;
  /// clear the entire model
  virtual void clear() = 0;

  /// return the last `count` first lines of commit messages in the model
  virtual QStringList messageHeads( int count = 10 ) const = 0;
  
  /// return the full test of the message at `index` in the model, where the most recent message is index = 0
  virtual QString message( int index ) const = 0;
};

#endif COMMITMESSAGEHISTORYMODELINTERFACE