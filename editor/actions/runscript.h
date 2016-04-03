#ifndef RUNSCRIPT_H
#define RUNSCRIPT_H

#include "action.h"

class Action;

class RunScript : public Action
{
    Q_OBJECT

    QString mScript;
    QString mComment;

public:
    explicit RunScript(QObject *parent = 0);
    RunScript(const QVariantMap& data, QObject *parent=0);
    virtual QVariantMap toJsonObject(bool internal=true) const;
    QString script() const;
    void setScript(const QString&);
    QString comment() const;
    void setComment(const QString&);
    virtual QString displayText() const;
    
signals:
    
public slots:

private:
    void init();
    
};

#endif // RUNSCRIPT_H
