#include "sockettimeout.h"

#include <QTimer>

SocketTimeout::SocketTimeout(QAbstractSocket* socket, int timeout, QObject *parent) :
    QObject(parent)
{
    mSocket = socket;
    if (mSocket) {
        connect(mSocket, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        connect(mSocket, SIGNAL(disconnected()), mSocket, SLOT(deleteLater()));
        QTimer::singleShot(timeout, this, SLOT(_timeout()));
    }
}

SocketTimeout::~SocketTimeout()
{
}

QAbstractSocket* SocketTimeout::socket() const
{
    return mSocket;
}

void SocketTimeout::_timeout()
{
    if (mSocket) {
        mSocket->disconnectFromHost();
    }
}
