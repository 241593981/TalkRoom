#ifndef CELL_MAIN_H
#define CELL_MAIN_H

#include <QWidget>
#include <QTcpSocket>
#include "dlg_setting.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Cell_Main; }
QT_END_NAMESPACE

class Cell_Main : public QWidget
{
    Q_OBJECT

public:
    Cell_Main(QWidget *parent = nullptr);
    ~Cell_Main();

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

    void dealClienRead();
    void dealPack(QByteArray arr);
    void tcpcon();

private slots:
    void on_btn_set_clicked();

    void on_btn_close_clicked();

    void on_btn_min_clicked();

    void on_btn_send_clicked();

    void on_btn_clear_clicked();

private:
    Ui::Cell_Main *ui;
    bool m_isPressed = false;

    QPoint m_wnd;
    QPoint press;
    Dlg_setting m_dlg;

    QTcpSocket m_socket;
    QByteArray m_buffer;

    int m_iPackLen;
};
#endif // CELL_MAIN_H
