#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTime>
#include <QtWebKitWidgets>
#include <QProcess>
#include <QTcpSocket>
#include<QStringList>
#include<QObject>


namespace Ui {
class Widget;
}

typedef struct gpsData
{
    char lo[16];
    char la[16];
}gpsDataSt;

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QByteArray GPS_RMC;
    int main();
    void convertGpsData(QString srcData, QString &destData, int pos);

public slots:

    void addObjectToJs();

private slots:
    void initButton();
    void openButton();
    void closeButton();
    void portFind();
    void mysleep(int);
    void carStop();

    void on_open_port_clicked();
    void on_close_port_clicked();
    void on_send_button_clicked();

    void on_clear_receive_button_clicked();
    void on_clear_send_button_clicked();

    void on_fore_button_clicked();
    void on_back_button_clicked();
    void on_left_button_clicked();
    void on_right_button_clicked();
    void on_up_button_clicked();
    void on_down_button_clicked();
    void on_location_button_clicked();
    void readData();


private:
    Ui::Widget *ui;
    QSerialPort *serialport;
    QProcess *mPorcess;



};

#endif // WIDGET_H
