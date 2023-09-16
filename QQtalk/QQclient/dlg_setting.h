#ifndef DLG_SETTING_H
#define DLG_SETTING_H

#include <QDialog>

namespace Ui {
class Dlg_setting;
}

class Dlg_setting : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_setting(QWidget *parent = nullptr);
    ~Dlg_setting();

    virtual void showEvent(QShowEvent *event);

    QString getLocalIP();
    QString getServerIP();
    int getServerPort();
    QString getName();
private slots:
    void on_btn_yes_clicked();

    void on_btn_cancel_clicked();

private:
    Ui::Dlg_setting *ui;
};

#endif // DLG_SETTING_H
