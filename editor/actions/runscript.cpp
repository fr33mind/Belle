#include "runscript.h"

RunScript::RunScript(QObject *parent) :
    Action(parent)
{
    init();
}

RunScript::RunScript(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("script") && data.value("script").type() == QVariant::String) {
        mScript = data.value("script").toString();
    }

    if (data.contains("comment") && data.value("comment").type() == QVariant::String) {
        mComment = data.value("comment").toString();
    }
}

void RunScript::init()
{
    setType(GameObjectMetaType::RunScript);
}

QVariantMap RunScript::toJsonObject(bool internal) const
{
    QVariantMap object = Action::toJsonObject(internal);
    object.insert("script", mScript);
    object.insert("comment", mComment);
    return object;
}

QString RunScript::script() const
{
    return mScript;
}

void RunScript::setScript(const QString& text)
{
   mScript = text;
   if (mComment.isEmpty() && mScript == displayText())
    emit dataChanged();
}

QString RunScript::comment() const
{
    return mComment;
}

void RunScript::setComment(const QString& text)
{
   mComment = text;
   emit dataChanged();
}

QString RunScript::displayText() const
{
    if (! mComment.isEmpty())
        return mComment;
    if (mScript.contains("\n")) {
       QStringList lines = mScript.split("\n").mid(0, 3);
       return lines.join("\n");
    }
    return mScript.mid(0, 20);
}
