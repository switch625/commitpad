#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QFrame>

class QLabel;

class TitleBar : public QFrame
{
  Q_OBJECT
public:
  explicit TitleBar(QWidget *parent = nullptr);
  virtual ~TitleBar();

  void setTitle( const QString &title );

protected:
  void mousePressEvent( QMouseEvent *event );
  void mouseMoveEvent( QMouseEvent *event );

private:
  QLabel *m_titleLabel;
  QPoint m_mouseDownPoint;
};

#endif // TITLEBAR_H
