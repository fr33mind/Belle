#include "numbervalidator.h"

static const QRegularExpression mStaticRegExp("^\\d+\\.?\\d*$");

NumberValidator::NumberValidator(QObject *parent) :
    QValidator(parent)
{
    mRegExp = QRegularExpression(mStaticRegExp);
}

QValidator::State NumberValidator::validate(QString & string, int & pos) const
{
    if (string.isEmpty())
        return Intermediate;

    QRegularExpressionMatch match = mRegExp.match(string);
    if (match.hasMatch())
        return Acceptable;
    return Invalid;
}

QValidator::State NumberValidator::validate(const QString & string) const
{
    int pos = 0;
    QString str(string);
    return validate(str, pos);
}

QRegularExpression NumberValidator::regularExpression()
{
    return mStaticRegExp;
}
