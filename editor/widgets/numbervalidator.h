#ifndef NUMBERVALIDATOR_H
#define NUMBERVALIDATOR_H

#include <QValidator>
#include <QRegularExpression>

class NumberValidator : public QValidator
{
    Q_OBJECT

public:
    explicit NumberValidator(QObject *parent = 0);
    virtual QValidator::State validate(QString &, int &) const;
    QValidator::State validate(const QString &) const;
    static QRegularExpression regularExpression();

signals:

public slots:

private:
    QRegularExpression mRegExp;
};

#endif // NUMBERVALIDATOR_H
