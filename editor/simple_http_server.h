#ifndef SIMPLE_HTTP_SERVER_H
#define SIMPLE_HTTP_SERVER_H

#include <QTcpServer>
#include <QDir>

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
    bool start();
    void stop();
    void setServerPort(qint64);
    void setServerDirectory(const QString&);
    QString serverDirectory();
    QString serverUrl();

protected:
    QString guessType(const QString&);
    QString fullPath(QString);

signals:

protected slots:
    void onNewConnection();
    void readClient();
    void discardClient();

};

#endif // SIMPLE_HTTP_SERVER_H
