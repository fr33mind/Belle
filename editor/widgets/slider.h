#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>

class Slider : public QSlider
{
    Q_OBJECT
public:
    explicit Slider(QWidget *parent = 0);
    Slider(Qt::Orientation, QWidget *parent = 0);
    virtual bool event(QEvent *e);

protected:
    virtual void wheelEvent(QWheelEvent *);

signals:

public slots:

};

#endif // SLIDER_H
