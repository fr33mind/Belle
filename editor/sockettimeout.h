#ifndef SOCKETTIMEOUT_H
#define SOCKETTIMEOUT_H

#include <QObject>
#include <QAbstractSocket>

class SocketTimeout : public QObject
{
    Q_OBJECT

public:
    SocketTimeout(QAbstractSocket*, int timeout, QObject *parent = 0);
    virtual ~SocketTimeout();
    QAbstractSocket* socket() const;

signals:

private slots:
    void _timeout();
    void onSocketDestroyed();

private:
    QAbstractSocket* mSocket;

};

#endif // SOCKETTIMEOUT_H
