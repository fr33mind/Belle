#include "slider.h"

#include <QEvent>
#include <QHelpEvent>
#include <QToolTip>

Slider::Slider(QWidget *parent) :
    QSlider(parent)
{
}

Slider::Slider(Qt::Orientation orientation, QWidget *parent) :
    QSlider(orientation, parent)
{
}

bool Slider::event(QEvent* e)
{
    if (e->type() == QEvent::ToolTip) {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
        QToolTip::showText(helpEvent->globalPos(), QString::number(value()), this);
        return true;
    }
    else if (e->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
        QToolTip::showText(mouseEvent->globalPos(), QString::number(value()), this);
    }
    else if (e->type() == QEvent::Enter) {
        QEnterEvent* enterEvent = static_cast<QEnterEvent*>(e);
        QToolTip::showText(enterEvent->globalPos(), QString::number(value()), this);
    }

    return QSlider::event(e);
}

void Slider::wheelEvent(QWheelEvent * event)
{
    event->ignore();
}
