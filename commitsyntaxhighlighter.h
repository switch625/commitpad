#ifndef COMMITSYNTAXHIGHLIGHTER_H
#define COMMITSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class CommitSyntaxHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT

public:
  CommitSyntaxHighlighter( QTextDocument *parent = nullptr );

  static QString jiraIssueKeySearchExpression() { return m_jiraKeySearchExpression; }

protected:
  void highlightBlock(const QString &text) override;

private:
  struct HighlightingRule
  {
    QRegularExpression pattern;
    QTextCharFormat format;
  };

  void addRule( const QStringList &patterns, const QTextCharFormat &format );

  QVector<HighlightingRule> m_highlightingRules;

  QTextCharFormat m_commentLineFormat;
  QTextCharFormat m_branchNameFormat;
  QTextCharFormat m_newFileFormat;
  QTextCharFormat m_modifiedFileFormat;
  QTextCharFormat m_removedFileFormat;
  QTextCharFormat m_jiraIssueFormat;

  static const QString m_jiraKeySearchExpression;
};

#endif // COMMITSYNTAXHIGHLIGHTER_H
