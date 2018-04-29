#include "stopallsounds.h"

StopAllSounds::StopAllSounds(QObject *parent) :
    Action(parent)
{
    init();
}

StopAllSounds::StopAllSounds(const QVariantMap& data,QObject *parent) :
    Action(data, parent)
{
    init();
    loadInternal(data);
}

void StopAllSounds::init()
{
    setType(GameObjectMetaType::StopAllSounds);
    mFadeTime = 0;
}

void StopAllSounds::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("fadeTime") && data.value("fadeTime").type() == QVariant::Double)
        setFadeTime(data.value("fadeTime").toDouble());
}

void StopAllSounds::setFadeTime(double time)
{
    if (mFadeTime == time)
        return;

    mFadeTime = time;
    notify("fadeTime", mFadeTime);
}

double StopAllSounds::fadeTime()
{
    return mFadeTime;
}

QVariantMap StopAllSounds::toJsonObject(bool internal) const
{
    QVariantMap data = Action::toJsonObject(internal);

    if (mFadeTime > 0.0)
        data.insert("fadeTime", mFadeTime);

    return data;
}
