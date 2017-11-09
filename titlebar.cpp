#include "titlebar.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMouseEvent>

TitleBar::TitleBar(QWidget *parent)
  : QFrame(parent)
{
  QHBoxLayout *layout = new QHBoxLayout( this );
  layout->setContentsMargins( 6, 6, 6, 6 );
  setLayout( layout );
  m_titleLabel = new QLabel( this );
  layout->addWidget( m_titleLabel );
  setStyleSheet( "background-color:#232343;color:#ffffff");
}

TitleBar::~TitleBar()
{
}

void TitleBar::setTitle( const QString &title )
{
  m_titleLabel->setText( title );
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
  if( event->button() == Qt::LeftButton )
  {
    m_mouseDownPoint = event->globalPos() - window()->geometry().topLeft();
    event->accept();
  }
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
  if(event->buttons() & Qt::LeftButton)
  {
    window()->move( event->globalPos() - m_mouseDownPoint );
    event->accept();
  }
}
