#ifndef COMMITPAD_H
#define COMMITPAD_H

#include <QMainWindow>
#include <QSet>
#include <QSettings>

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
    Ui::CommitPad *ui;
    Result m_result;
    QString m_filename;
    QSet< QString > m_commitMessageFilenames;
};

#endif // COMMITPAD_H
