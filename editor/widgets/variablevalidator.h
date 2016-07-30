#ifndef VARIABLEVALIDATOR_H
#define VARIABLEVALIDATOR_H

#include <QValidator>
#include <QRegularExpression>

class VariableValidator : public QValidator
{
    Q_OBJECT
public:
    explicit VariableValidator(QObject *parent = 0);
    virtual QValidator::State validate(QString &, int &) const;
    QValidator::State validate(const QString &) const;
    static QRegularExpression regularExpression();

signals:

public slots:

private:
    QRegularExpression mRegExp;

};

#endif // VARIABLEVALIDATOR_H
