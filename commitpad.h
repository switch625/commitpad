#ifndef COMMITPAD_H
#define COMMITPAD_H

#include <QMainWindow>

namespace Ui {
class CommitPad;
}

class CommitPad : public QMainWindow
{
    Q_OBJECT

public:
    explicit CommitPad(QWidget *parent = 0);
    ~CommitPad();

protected:
    void loadFile( const QString &filename );
    void updateToolBar();

protected slots:
    void commit();
    void cancel();

    void onInsertJiraKey( const QString &key );

signals:
    void warningMsg( const QString &warning );

private:
    Ui::CommitPad *ui;
    QString m_filename;
};

#endif // COMMITPAD_H
