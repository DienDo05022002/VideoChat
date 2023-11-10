#include "websocket_client.h"

//WebsocketClient::WebsocketClient()
//{

//}
#include "qjsondocument.h"
WebsocketClient::WebsocketClient(QObject *parent)
    : QObject{parent}
{
    m_webSocket=new Data_Socket();
    m_myid=-1;
    connect(m_webSocket,&Data_Socket::newTextMessageReceived,this,&WebsocketClient::onTextMessageReceived);
}

void WebsocketClient::setUsername(QString username)
{
    m_username=username;
}

QString WebsocketClient::getUsername()
{
    return m_username;
}

void WebsocketClient::setMyId(int peerid)
{
    m_myid=peerid;
}

int WebsocketClient::getMyId()
{
    return m_myid;
}

bool WebsocketClient::isConnected()
{
    return m_myid!=-1;
}

void WebsocketClient::startLogin(QString serverIp, int port)
{
    qDebug()<<serverIp<<" port:"<<port;
    QUrl url(QString("ws://").append(serverIp).append(QString(":")).append(QString::number(port)));
    //    QUrl url("ws://127.0.0.1:4567");
    m_webSocket->connectToServer(url);
    connect(m_webSocket,&Data_Socket::connected,this,&WebsocketClient::sendLoginText);
}

void WebsocketClient::signOut()
{

    m_webSocket->disconnect();
}

QJsonArray WebsocketClient::getPeerList()
{
    QJsonArray arr;
    for(auto it=m_peerlist.constBegin();it!=m_peerlist.constEnd();++it)
    {
        QJsonObject obj;
        obj.insert("id",it.key());
        obj.insert("name",it.value());
        arr.append(obj);
    }
    return arr;
}
void WebsocketClient::sendLoginText()
{
    QJsonObject loginJsonObj;
    loginJsonObj.insert("type","signin");
    loginJsonObj.insert("username",m_username);
    QJsonDocument doc(loginJsonObj);
    qDebug()<<doc;
    m_webSocket->sendTextToServer(doc.toJson(QJsonDocument::Compact));
    Q_EMIT loginSuccess();
}

void WebsocketClient::onTextMessageReceived(const QString &message)
{
    QJsonParseError error;
    QJsonDocument jsonDocument=QJsonDocument::fromJson(message.toUtf8(),&error);
    if(error.error||!jsonDocument.isObject()) {
        qDebug()<<"Receive a message which can not be converted to JSON format";
        return;
    }
    QJsonObject jsonObj=jsonDocument.object();
    QString type=jsonObj.value("type").toString();
    if(type=="client")
    {
        QString username=jsonObj.value("username").toString();
        int peerid=jsonObj.value("peerid").toInt();
        if(!m_peerlist[peerid].isEmpty()) {
            qDebug()<<"There is a new client ,but it's id duplicate with an id in the list.Didn't not process it.";
            return;
        }
        m_peerlist[peerid]=username;
        Q_EMIT peerConnectedOrDisconnected();
    }else if(type=="out") {
        int peerid=jsonObj.value("peerid").toInt();
        if(m_peerlist[peerid].isEmpty()) {
            qDebug()<<"There is a  client sign out,but its id isn't in the list.Didn't not process it.";
            return;
        }
        m_peerlist.remove(peerid);
        Q_EMIT peerConnectedOrDisconnected();
    }else if(type=="loginresponse") {
        int myid=jsonObj.value("peerid").toInt();
        m_myid=myid;
        Q_EMIT sendId(m_myid);
    }else if(type=="message") {
        Q_EMIT messageFromPeer(jsonObj);
    }
}

void WebsocketClient::sendToPeer(const QString& message)
{
    m_webSocket->sendTextToServer(message);
}
