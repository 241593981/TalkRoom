#include "dlg_setting.h"
#include "ui_dlg_setting.h"
#include <QtDebug>
#include <QNetworkInterface.h>

Dlg_setting::Dlg_setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_setting)
{
    ui->setupUi(this);
}

Dlg_setting::~Dlg_setting()
{
    delete ui;
}

void Dlg_setting::showEvent(QShowEvent *event)
{
    qDebug()<<"dlg show";
    ui->cb_ips->clear();
    auto l = QNetworkInterface::allAddresses();
    for(auto it:l)
    {
        if(QAbstractSocket::IPv4Protocol == it.protocol())
        {
            ui->cb_ips->addItem(it.toString());
        }
    }
}

QString Dlg_setting::getLocalIP()
{
    return ui->cb_ips->currentText();
}

QString Dlg_setting::getServerIP()
{
    return ui->le_server->text();
}

int Dlg_setting::getServerPort()
{
    return ui->le_port->text().toInt();
}

QString Dlg_setting::getName()
{
    return ui->le_name->text();
}

void Dlg_setting::on_btn_yes_clicked()
{
    this->done(1);
}

void Dlg_setting::on_btn_cancel_clicked()
{
    this->done(0);
}
