#include "cell_main.h"
#include "ui_cell_main.h"
#include <QMouseEvent>
#include <QtDebug>
#include <QDateTime>
#include <qhostaddress.h>

Cell_Main::Cell_Main(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Cell_Main)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    auto fcon= [&](){
        auto ip = m_socket.localAddress().toString();
        ui->textBrowser->append(ip+"连接成功");
    };
    connect(&m_socket,&QAbstractSocket::connected,this,fcon);

    auto fdiscon= [&](){
        auto ip = m_socket.localAddress().toString();
        ui->textBrowser->append(ip+"断开链接");
    };
    connect(&m_socket,&QAbstractSocket::connected,this,fdiscon);
    connect(&m_socket,&QAbstractSocket::readyRead,this,Cell_Main::dealClienRead);

    ui->lb_name->setText(m_dlg.getName());
}

Cell_Main::~Cell_Main()
{
    delete ui;
}

void Cell_Main::mousePressEvent(QMouseEvent *event)
{
    auto rect = ui->title->geometry();
    auto p = event->pos();
    if(rect.contains(p))
    {
        m_isPressed = true;
        m_wnd = this->geometry().topLeft();
        press = this->mapToGlobal(p);
    }
}

void Cell_Main::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
}

void Cell_Main::mouseMoveEvent(QMouseEvent *event)
{
    if(m_isPressed)
    {
        auto v = mapToGlobal(event->pos()) - press;
        auto r = m_wnd;
        this->move(r+v);
    }
}

void Cell_Main::dealClienRead()
{
    m_buffer.push_back(m_socket.readAll());
    if(m_buffer.size()>=4 && m_buffer[0].operator==(0x66))
    {
        quint16 len = (m_buffer[2] << 8 & 0xFF00) || static_cast<quint16>(m_buffer[1]);
        if(m_buffer.size()>=len)
        {
            QByteArray arr = m_buffer.mid(0,len);
            dealPack(arr);
            m_buffer = m_buffer.mid(len);
        }
    }
}

void Cell_Main::dealPack(QByteArray arr)
{
    char cmd = arr[3];
    QByteArray arrData = arr.mid(4);
    do
    {
        if(cmd == 0x01)
        {
            break;
        }

        if(cmd == 0x02)
        {
            ui->textBrowser->append("<font color = 'green'>我发送:" + arrData);
            break;
        }

        auto strTime = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        if(cmd == 0x03)
        {

            ui->textBrowser->append("<font color = 'blue'>接受数据:" + arrData);
            break;
        }
    }while(false);
}

void Cell_Main::tcpcon()
{
    auto local_ip = m_dlg.getLocalIP();
    auto server_port = m_dlg.getServerPort();
    auto server_ip = m_dlg.getServerIP();
    m_socket.close();
    m_socket.bind(QHostAddress(local_ip));
    m_socket.connectToHost(server_ip,server_port);

}

void Cell_Main::on_btn_set_clicked()
{
    int ret = m_dlg.exec();
    if(ret)
    {
        tcpcon();
        qDebug()<<"local ip:"<<m_dlg.getLocalIP();
        qDebug()<<"server ip:"<<m_dlg.getServerIP();
        qDebug()<<"server port:"<<m_dlg.getServerPort();
        ui->lb_name->setText(m_dlg.getName());
    }
}

void Cell_Main::on_btn_close_clicked()
{
    exit(0);
}

void Cell_Main::on_btn_min_clicked()
{
    showMinimized();
}

void Cell_Main::on_btn_send_clicked()
{
    auto str = ui->textEdit->toPlainText();
    ui->textEdit->clear();    

    QByteArray arr = str.toUtf8();
    quint16 l = arr.size()+4;
    arr.insert(0,0x66);
    arr.insert(1,static_cast<char>(l&0x00FF));
    arr.insert(2,static_cast<char>(l>>8&0x00FF));
    arr.insert(3,0x66);
    m_socket.write(arr);
}

void Cell_Main::on_btn_clear_clicked()
{
    ui->textBrowser->clear();
}
