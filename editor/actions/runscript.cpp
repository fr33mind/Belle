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
    loadInternal(data);
}

void RunScript::init()
{
    setType(GameObjectMetaType::RunScript);
}

void RunScript::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("script") && data.value("script").type() == QVariant::String) {
        setScript(data.value("script").toString());
    }

    if (data.contains("comment") && data.value("comment").type() == QVariant::String) {
        setComment(data.value("comment").toString());
    }
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
   if (mScript == text)
       return;

   mScript = text;
   notify("script", mScript);
}

QString RunScript::comment() const
{
    return mComment;
}

void RunScript::setComment(const QString& text)
{
   if (mComment == text)
       return;

   mComment = text;
   notify("comment", mComment);
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
