#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <QObject>
#include "data_socket.h"
//#include <QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>
class WebsocketClient : public QObject
{
    Q_OBJECT
public:
    explicit WebsocketClient(QObject *parent = nullptr);
    bool isConnected();
    Q_INVOKABLE void setUsername(QString username);
    Q_INVOKABLE QString getUsername();
    Q_INVOKABLE void setMyId(int peerid);
    Q_INVOKABLE int getMyId();

    Q_INVOKABLE void startLogin(QString serverIp,int port);
    Q_INVOKABLE void signOut();
    Q_INVOKABLE QJsonArray getPeerList();
    //    Q_INVOKABLE void
    void sendToPeer(const QString& message);
public Q_SLOTS:
    void sendLoginText();
    void onTextMessageReceived(const QString& message);
Q_SIGNALS:
    void sendId(int myid);
    void login();
    void peerConnectedOrDisconnected();
    void loginSuccess();
    void messageFromPeer(QJsonObject jsonObj);
private:
    Data_Socket* m_webSocket;
    QString m_username;
    int m_myid;
    QMap<int,QString> m_peerlist;
};

#endif // WEBSOCKETCLIENT_H
