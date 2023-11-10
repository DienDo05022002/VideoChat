#include "data_socket.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


Data_Socket::Data_Socket(QObject *parent)
    : QObject{parent}
{
    qDebug() << "Data_Socket::Data_Socket";
    init();
}

Data_Socket::~Data_Socket()
{
    qDebug() << "Data_Socket::~Data_Socket";
    if(m_webSocket) {
        m_webSocket->close();
        m_webSocket->deleteLater();
        m_webSocket=nullptr;
    }
}

void Data_Socket::connectToServer(const QUrl url)
{
qDebug() << "Data_Socket::connectToServer";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ << url;
#endif
    m_webSocket->open(url);
}

void Data_Socket::sendTextToServer(const QString message)
{
qDebug() << "Data_Socket::sendTextToServer";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ << message;
#endif
    int bytes=m_webSocket->sendTextMessage(message);
    qDebug()<<"Data_Socket(websocketclient) sended byte counts:"<<bytes;
}

void Data_Socket::sendBinaryToServer(const QByteArray message)
{
qDebug() << "Data_Socket::sendBinaryToServer";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ << message;
#endif
    m_webSocket->sendBinaryMessage(message);
}

void Data_Socket::init()
{
    qDebug() << "Data_Socket::init";
    m_webSocket=new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, this, &Data_Socket::onConnected);
    connect(m_webSocket, &QWebSocket::connected, this, &Data_Socket::connected);
    connect(m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
            this, &Data_Socket::onSslErrors);
}

void Data_Socket::onConnected()
{
qDebug() << "Data_Socket::onConnected";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ ;
#endif
    qDebug() << "WebSocket connected";
    connect(m_webSocket, &QWebSocket::textMessageReceived,
            this, &Data_Socket::newTextMessageReceived, Qt::QueuedConnection);
    connect(m_webSocket, &QWebSocket::binaryMessageReceived,
            this, &Data_Socket::newBinaryMessageReceived, Qt::QueuedConnection);
}

void Data_Socket::onSslErrors(const QList<QSslError> &errors)
{
qDebug() << "Data_Socket::onSslErrors";
#ifndef QT_NO_DEBUG
    qInfo()<<Q_FUNC_INFO<< __LINE__ ;
#endif
    Q_UNUSED(errors);

    // WARNING: Never ignore SSL errors in production code.
    // The proper way to handle self-signed certificates is to add a custom root
    // to the CA store.

    m_webSocket->ignoreSslErrors();
}
