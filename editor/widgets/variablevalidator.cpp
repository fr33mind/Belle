#include "variablevalidator.h"

static QRegularExpression mStaticRegExp("^[a-zA-Z_][a-zA-Z0-9_]*$");

VariableValidator::VariableValidator(QObject *parent) :
    QValidator(parent)
{
    mRegExp = QRegularExpression(mStaticRegExp);
}

QValidator::State VariableValidator::validate(QString & string, int & pos) const
{
    if (string.isEmpty())
        return Intermediate;

    QRegularExpressionMatch match = mRegExp.match(string);
    if (match.hasMatch())
        return Acceptable;
    return Invalid;
}

QValidator::State VariableValidator::validate(const QString & string) const
{
    int pos = 0;
    QString str(string);
    return validate(str, pos);
}

QRegularExpression VariableValidator::regularExpression()
{
    return mStaticRegExp;
}
