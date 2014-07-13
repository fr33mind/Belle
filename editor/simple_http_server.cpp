#include "simple_http_server.h"

#include <QTcpSocket>
#include <QDateTime>
#include <QRegExp>
#include <QStringList>
#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>

SimpleHttpServer::SimpleHttpServer(const QString& address, int port, const QString& dir, QObject *parent) :
    QTcpServer(parent)
{
    mAddress = address;
    mPort = port;
    mDirectory = QDir(dir);

    mHttpPorts << 591 << 8008 << 8080 << 8081 << 8090; //common port alternatives for HTTP

    mMimetypes.insert("", "application/octet-stream");
    mMimetypes.insert("html", "text/html");
    mMimetypes.insert("htm", "text/html");
    mMimetypes.insert("js", "text/javascript");
    mMimetypes.insert("css", "text/css");
    mMimetypes.insert("json", "application/json");
    mMimetypes.insert("xml", "text/xml");
    mMimetypes.insert("gif", "image/gif");
    mMimetypes.insert("jpeg", "image/jpeg");
    mMimetypes.insert("jpg", "image/jpeg");
    mMimetypes.insert("png", "image/png");
    mMimetypes.insert("svg", "image/svg+xml");
    mMimetypes.insert("tiff", "image/tiff");

    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

void SimpleHttpServer::onNewConnection()
{
    if (hasPendingConnections()) {
        QTcpSocket* socket = nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(readClient()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(discardClient()));
    }
}

void SimpleHttpServer::readClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();

    if (socket->canReadLine()) {
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));

        if (tokens[0] == "GET" && tokens.size() > 1) {
            QStringList header;
            QString file = tokens[1];
            QString path = fullPath(file);
            QByteArray data = QString("No index.html or index.htm found on \"%1\". :(").arg(mDirectory.absolutePath()).toAscii();
            QString ctype = "text/plain";
            QString modified = "";
            QString charset("");
            if (ctype.contains("text") || ctype.contains("json"))
                charset = "; charset=\"utf-8\"";

            if (! path.isEmpty()) {
                ctype = guessType(path);
                QFileInfo info(path);
                QDateTime date = info.lastModified();
                modified = QString("%1 %2").arg(date.toString("ddd, dd MMM yyyy hh:mm:ss")).arg("GMT");
                data = readFile(path);
            }

            if (file != "/" && data.size() <= 0) {
                header << "HTTP/1.0 404 Not Found\r\n" << "\r\n";
            }
            else {
                 header << "HTTP/1.0 200 Ok\r\n"
                 << QString("Content-Type: %1\r\n").arg(ctype)
                 << QString("Content-Length: %1\r\n").arg(data.size())
                 << QString("Last-Modified: %1\r\n").arg(modified)
                 << "\r\n";
            }

            socket->write(header.join("").toAscii());
            if (data.size())
                socket->write(data);
            socket->disconnectFromHost();

            if (socket->state() == QTcpSocket::UnconnectedState)
                socket->deleteLater();
        }
    }
}

QByteArray SimpleHttpServer::readFile(const QString& filePath)
{
    QFile file(filePath);
    QByteArray data;
    if (! file.open(QFile::ReadOnly))
        return data;

    data = file.readAll();
    file.close();
    return data;
}

void SimpleHttpServer::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}

QString SimpleHttpServer::guessType(const QString& path)
{
    QFileInfo info(path);
    QString ext = info.suffix();

    if (! mMimetypes.contains(ext))
        ext = ext.toLower();
    //if (! mMimetypes.contains(ext))
    //    mMimetypes.insert(ext, "text/" + ext);

    return mMimetypes.value(ext, "application/octet-stream");
}

bool SimpleHttpServer::start()
{
    if (isListening())
        return true;

    QHostAddress address(mAddress);
    if(! listen(address, mPort)) {
        //try common http ports
        foreach(int port, mHttpPorts) {
            if (listen(address, port))
                break;
        }

        //as last resource let Qt choose a random port
        if (! isListening())
            listen(address, 0);
    }

    mPort = serverPort();
    qDebug() << "Serving HTTP on" << address.toString() << "port" << mPort;

    return isListening();
}

void SimpleHttpServer::stop()
{
    close();
}

void SimpleHttpServer::setServerDirectory(const QString & path)
{
    mDirectory.setPath(path);
}

QString SimpleHttpServer::serverDirectory()
{
    return mDirectory.absolutePath();
}

QString SimpleHttpServer::fullPath(QString path)
{
    path = path.split('?')[0];
    path = path.split('#')[0];
    path = QUrl::fromPercentEncoding(path.toAscii());
    path = QDir::cleanPath(path);
    QStringList words = path.split('/', QString::SkipEmptyParts);
    QDir dir = mDirectory;
    QFileInfo info = mDirectory.absolutePath();
    foreach(QString word, words) {
        info.setFile(dir, word);
        if (info.isDir())
            dir.cd(word);
    }

    if (info.isDir()) {
        if (dir.exists("index.html"))
            return dir.absoluteFilePath("index.html");
        else if (dir.exists("index.htm"))
            return dir.absoluteFilePath("index.htm");
        else
            return "";
    }

    return info.absoluteFilePath();
}

void SimpleHttpServer::setServerPort(qint64 port)
{
    mPort = port;
}

QString SimpleHttpServer::serverUrl()
{
    return QString("http://%1:%2").arg(serverAddress().toString()).arg(serverPort());
}
