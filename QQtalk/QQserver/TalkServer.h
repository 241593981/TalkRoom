#ifndef TALKSERVER_H
#define TALKSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QList>

class TalkServer:public QObject
{
    Q_OBJECT
public:
    TalkServer(QObject *parent = nullptr):QObject(parent)
    {

    }
    ~TalkServer()
    {

    }
    void init()
    {
        m_tcpserver.listen(QHostAddress("192.168.10.6"),8899);
        connect(&m_tcpserver,&QTcpServer::newConnection,this,&TalkServer::dealNewCon);
        qDebug()<<"server running";
    }
    void dealNewCon()
    {

        auto *socket = m_tcpserver.nextPendingConnection();
        qDebug()<<"new con"<<socket->peerAddress().toString();
        TcpInfo info{socket};
        m_ltcp<<info;
        connect(socket,&QTcpSocket::readyRead,this,&TalkServer::dealRead);
        connect(socket,&QTcpSocket::readyRead,this,&TalkServer::dealbye);
    }

    void dealbye()
    {
        QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
        qDebug()<<"socket close"<<socket->peerAddress().toString();
        for(int i = 0; i < m_ltcp.size(); i++)
        {
            if(m_ltcp[i].socket == socket)
            {
                m_ltcp.erase(m_ltcp.begin()+i);
                break;
            }
        }
    }
    void dealRead()
    {
        QTcpSocket *socket = static_cast<QTcpSocket*>(sender());
        auto data = socket->readAll();
        for(int i = 0; i < m_ltcp.size(); i++)
        {
            if(m_ltcp[i].socket == socket)
            {
                m_ltcp[i].arrData.append(data);
                dealPack(i);
                break;
            }
        }
    }
    void dealPack(int i)
    {
        auto info = &m_ltcp[i];
        if(info->arrData.size()>=4 && info->arrData[0].operator==(0x66))
        {
            quint16 len = (info->arrData[2] << 8 & 0xFF00) || static_cast<quint16>(info->arrData[1]);
            if(info->arrData.size()>=len)
            {
                QByteArray arr = info->arrData.mid(0,len);
                dealRec(i, arr);
                info->arrData = info->arrData.mid(len);
            }
        }
    }

    void dealRec(int i, QByteArray arr)
    {
        auto info = m_ltcp[i];

        char cmd = arr[3];
        QByteArray arrData = arr.mid(4);
        do
        {
            if(cmd == 0x01)
            {
                break;
            }
            if(cmd == 0x03)
            {
                auto strIp = info.socket->peerAddress().toString()+":";
                arrData.insert(0,strIp.toUtf8());
                qDebug()<<strIp<<":"<<QString(arrData);
                QByteArray arrSendBuffer;
                arrSendBuffer.append(0x66);
                quint16 l = arrData.size()+4;
                arrSendBuffer.append(static_cast<char>(l&0x00FF));
                arrSendBuffer.append(static_cast<char>(l>>8&0x00FF));
                arrSendBuffer.append(0x03);
                arrSendBuffer.append(arrData);
                
                for(int i = 0; i < m_ltcp.size(); i++)
                {
                    if(m_ltcp[i].socket == info.socket)
                    {
                        arrSendBuffer[3] = 0x02;
                        m_ltcp[i].socket->write(arrSendBuffer);
                        continue;
                    }
                    arrSendBuffer[3] = 0x03;
                    m_ltcp[i].socket->write(arrSendBuffer);
                }
                break;
            }
        }while(false);
    }
private:
    QTcpServer m_tcpserver;
    struct TcpInfo
    {
        QTcpSocket *socket;
        QByteArray arrData;
    };

    QList<TcpInfo> m_ltcp;
};

#endif // TALKSERVER_H
