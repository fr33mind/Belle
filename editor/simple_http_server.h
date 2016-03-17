#ifndef SIMPLE_HTTP_SERVER_H
#define SIMPLE_HTTP_SERVER_H

#include <QTcpServer>
#include <QDir>
#include <QTimer>
#include <QTcpSocket>

const quint32 KEEP_ALIVE_TIMEOUT = 5;

class SimpleHttpServer : public QTcpServer
{
    Q_OBJECT

    QHash<QString, QString> mMimetypes;
    QString mAddress;
    qint64 mPort;
    QDir mDirectory;
    QList<int> mHttpPorts;

public:
    explicit SimpleHttpServer(const QString& address="127.0.0.1", int port=0, const QString& dir=".", QObject *parent = 0);
    QByteArray readFile(const QString&);
    QByteArray readDir(const QString&);
    bool start();
    void stop();
    void setServerPort(qint64);
    void setServerDirectory(const QString&);
    QString serverDirectory();
    QString serverUrl();

protected:
    QString guessMimeType(const QFileInfo&);
    QString httpDate(const QDateTime&);
    QFileInfo fileInfo(const QString&);

signals:

protected slots:
    void onNewConnection();
    void readClient();
    void discardClient();

};

#endif // SIMPLE_HTTP_SERVER_H
