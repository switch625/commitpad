#ifndef COMMITPAD_H
#define COMMITPAD_H

#include <QMainWindow>
#include <QSet>
#include <QSettings>
#include <QStack>

namespace Ui {
class CommitPad;
}

class CommitPad : public QMainWindow
{
    Q_OBJECT

public:
    explicit CommitPad( QSettings &settings, QWidget *parent = 0 );
    ~CommitPad();

protected:
    void loadFile( const QString &filename );
    void updateToolBar();

    void showEvent( QShowEvent *event );
    void closeEvent( QCloseEvent *event );

    QIcon generateActionIcon( const QString &iconFilename );

protected slots:
    void onCommit();
    void onCancel();
    void onSettingsButtonClicked();
    void onInsertJiraKey( const QString &key );

signals:
    void warningMsg( const QString &warning );

private:
    enum Result
    {
      Undefined,
      Accept,
      Reject
    };
    QSettings &m_settings;
    QStack< QString > m_history;
    Ui::CommitPad *ui;
    Result m_result;
    QString m_filename;
    QHash< QString, QIcon > m_operationIcons;
};

#endif // COMMITPAD_H
