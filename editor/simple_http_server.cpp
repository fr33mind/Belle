#include "simple_http_server.h"

#include <QTcpSocket>
#include <QDateTime>
#include <QRegExp>
#include <QStringList>
#include <QtDebug>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QLocale>

#include "sockettimeout.h"

SimpleHttpServer::SimpleHttpServer(const QString& address, int port, const QString& dir, QObject *parent) :
    QTcpServer(parent)
{
    mAddress = address;
    mPort = port;
    mDirectory = QDir(dir);

    mHttpPorts << 591 << 8008 << 8080 << 8081 << 8090; //common port alternatives for HTTP

    mMimetypes.insert("", "application/octet-stream");
    mMimetypes.insert("ogg", "audio/ogg");
    mMimetypes.insert("oga", "audio/ogg");
    mMimetypes.insert("mp3", "audio/mpeg");
    mMimetypes.insert("html", "text/html");
    mMimetypes.insert("htm", "text/html");
    mMimetypes.insert("js", "application/javascript");
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
    }
}

void SimpleHttpServer::readClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();

    if (socket->canReadLine()) {
        QMap<QString, QString> headers;
        QString header_line;
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        int range_start = 0, range_end = 0;
        bool isRanged = false;
        bool keepAlive = true;
        QString value;

        while(socket->canReadLine()) {
            header_line = socket->readLine().trimmed();
            if (header_line.contains(":")) {
                QStringList parts = header_line.split(":");
                headers[parts[0].trimmed().toLower()] = parts[1].trimmed();
            }
        }

        if (headers.contains("range")) {
            value = headers.value("range", "");
            if (value.contains("-")) {
                QString range = value.replace("bytes=", "");
                range_start = range.split("-")[0].toInt();
                range_end = range.split("-")[1].toInt();
                isRanged = true;
            }
        }

        if (headers.contains("connection")) {
            value = headers.value("connection");
            if (value.toLower() == "close")
                keepAlive = false;
        }

        if (tokens.size() > 1 && tokens[0] == "GET") {
            QStringList header;
            QString file = tokens[1];
            QFileInfo fileInfo = this->fileInfo(file);
            QByteArray data;
            QString mimetype = "text/html";
            QString rangeinfo("");

            if (!fileInfo.exists()) {
                header << "HTTP/1.1 404 Not Found";
                header << QString("Content-Type: %1").arg(mimetype);
                header << "Connection: close";
                keepAlive = false;
            }
            else {
                if (fileInfo.isFile()) {
                    mimetype = guessMimeType(fileInfo);
                    data = readFile(fileInfo.absoluteFilePath());
                    if (isRanged) {
                        int size = data.size();
                        if (!range_end)
                            range_end = size - 1;

                        data = data.mid(range_start, (range_end - range_start)+1);
                        rangeinfo = QString("bytes %1-%2/%3").arg(range_start).arg(range_end).arg(size);
                    }
                }
                else if (fileInfo.isDir()) {
                    data = readDir(fileInfo.absoluteFilePath());
                }

                if (isRanged) {
                    header << "HTTP/1.1 206 Partial Content";
                    header << QString("Accept-Ranges: bytes");
                    header << QString("Content-Range: %1").arg(rangeinfo);
                }
                else {
                    header << "HTTP/1.1 200 Ok";
                }

                header << QString("Content-Type: %1").arg(mimetype);
                header << QString("Content-Length: %1").arg(data.size());
                header << QString("Last-Modified: %1").arg(httpDate(fileInfo.lastModified()));
                if (keepAlive)
                    header << "Connection: keep-alive";
            }

            header << "Server: Belle/0.7";
            header << QString("Date: %1").arg(httpDate(QDateTime::currentDateTime()));
            header << "\r\n";

            socket->write(header.join("\r\n").toUtf8());
            if (data.size())
                socket->write(data);

            quint32 timeout = 0;
            if (keepAlive)
                timeout = KEEP_ALIVE_TIMEOUT * 1000;

            new SocketTimeout(socket, timeout, this);
        }
    }
}

QByteArray SimpleHttpServer::readFile(const QString& filePath)
{
    QFile file(filePath);
    QByteArray data;

    if (! file.exists())
        return data;

    if (! file.open(QFile::ReadOnly))
        return data;

    data = file.readAll();
    file.close();
    return data;
}

QByteArray SimpleHttpServer::readDir(const QString& dirPath)
{
    QDir dir(dirPath);
    QByteArray data;

    if (! dir.exists())
        return data;

    if (dir.exists("index.html"))
        data = readFile(dir.absoluteFilePath("index.html"));
    else if (dir.exists("index.htm"))
        data = readFile(dir.absoluteFilePath("index.htm"));
    else
        data = QString("No index.html or index.htm found at \"%1\".").arg(mDirectory.absolutePath()).toUtf8();

    return data;
}

void SimpleHttpServer::discardClient()
{
    QTcpSocket* socket = (QTcpSocket*)sender();
    socket->deleteLater();
}

QString SimpleHttpServer::guessMimeType(const QFileInfo& fileInfo)
{
    QString ext = fileInfo.suffix();

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

QFileInfo SimpleHttpServer::fileInfo(const QString& filepath)
{
    QString path = filepath;
    path = path.split('?')[0];
    path = path.split('#')[0];
    path = QUrl::fromPercentEncoding(path.toUtf8());
    path = QDir::cleanPath(path);
    if (path.startsWith("/"))
        path.remove(0, 1);

    QFileInfo info(mDirectory.absoluteFilePath(path));
    return info;
}

void SimpleHttpServer::setServerPort(qint64 port)
{
    mPort = port;
}

QString SimpleHttpServer::serverUrl()
{
    return QString("http://%1:%2").arg(serverAddress().toString()).arg(serverPort());
}

QString SimpleHttpServer::httpDate(const QDateTime & date)
{
    QLocale en(QLocale::English);
    QDateTime utcDate = date.toUTC();
    int weekDay = utcDate.date().dayOfWeek();
    int month = utcDate.date().month();

    return QString("%1, %2 %3 %4 GMT").arg(en.dayName(weekDay, QLocale::ShortFormat))
                                         .arg(utcDate.toString("dd"))
                                         .arg(en.monthName(month, QLocale::ShortFormat))
                                         .arg(utcDate.toString("yyyy hh:mm:ss"));
}
