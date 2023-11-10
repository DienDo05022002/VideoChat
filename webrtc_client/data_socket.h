#ifndef DATA_SOCKET_H
#define DATA_SOCKET_H

#include <QObject>
#include <QSslError>
#include <QWebSocket>

class Data_Socket : public QObject
{
    Q_OBJECT
public:
    explicit Data_Socket(QObject *parent = nullptr);
    ~Data_Socket();
    void connectToServer(const QUrl url);

    void sendTextToServer(const QString message);

    void sendBinaryToServer(const QByteArray message);
Q_SIGNALS:
    void error(const QString errorString, const QString detailString = QString());

    void connected();

    void newTextMessageReceived(QString message);
    void newBinaryMessageReceived(QByteArray message);

private:
    void init();

    void onConnected();
    //    void onTextMessageReceived(QString message);
    //    void onBinaryMessageReceived(QByteArray message);
    void onSslErrors(const QList<QSslError> &errors);

    QWebSocket* m_webSocket;
    QString m_url = "ws://127.0.0.1:4567";
};

#endif // DATA_SOCKET_H
