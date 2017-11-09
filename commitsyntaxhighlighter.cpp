#include "commitsyntaxhighlighter.h"

const QString CommitSyntaxHighlighter::m_jiraKeySearchExpression( "[A-Z]{1,10}\\-[0-9]{1,6}" );

CommitSyntaxHighlighter::CommitSyntaxHighlighter(QTextDocument *parent)
  : QSyntaxHighlighter( parent )
{
  m_commentLineFormat.setForeground( Qt::darkGray );
  m_commentLineFormat.setFontItalic( true );
  QStringList patterns;
  patterns << "^\\#.*";
  addRule( patterns, m_commentLineFormat );
  patterns.clear();

  m_jiraIssueFormat.setFontUnderline( true );
  patterns << m_jiraKeySearchExpression;
  addRule( patterns, m_jiraIssueFormat );
  patterns.clear();

  m_branchNameFormat.setForeground( QColor( 0x26a9e0 ) );
  patterns << "(?<=(On branch)).+";
  addRule( patterns, m_branchNameFormat );
  patterns.clear();

  m_newFileFormat.setForeground( QColor( 0x56c96d ) );
  patterns << "(?<=(new file:)).+";
  addRule( patterns, m_newFileFormat );
  patterns.clear();

  m_modifiedFileFormat.setForeground( QColor( 0x9a569a ) );
  patterns << "(?<=(modified:)).+";
  addRule( patterns, m_modifiedFileFormat );
  patterns.clear();

  m_removedFileFormat.setForeground( QColor( 0xff5663 ) );
  patterns << "(?<=(deleted:)).+";
  addRule( patterns, m_removedFileFormat );
  patterns.clear();
}

void CommitSyntaxHighlighter::highlightBlock(const QString &text)
{
  foreach(const HighlightingRule &rule, m_highlightingRules)
  {
    QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
    while(matchIterator.hasNext())
    {
      QRegularExpressionMatch match = matchIterator.next();
      setFormat(match.capturedStart(), match.capturedLength(), rule.format);
    }
  }
}

void CommitSyntaxHighlighter::addRule(const QStringList &patterns, const QTextCharFormat &format)
{
  HighlightingRule rule;
  foreach(const QString &pattern, patterns)
  {
    rule.pattern = QRegularExpression(pattern);
    rule.format = format;
    m_highlightingRules.append(rule);
  }
}
