#ifndef STOPALLSOUNDS_H
#define STOPALLSOUNDS_H

#include "action.h"

class StopAllSounds : public Action
{
    Q_OBJECT

   double mFadeTime;

public:
    explicit StopAllSounds(QObject *parent = 0);
    StopAllSounds(const QVariantMap&, QObject *parent = 0);

    double fadeTime();
    void setFadeTime(double);

    virtual QVariantMap toJsonObject(bool internal=true) const;

signals:

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private:
    void init();
};

#endif // STOPALLSOUNDS_H
