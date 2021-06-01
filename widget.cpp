#include "widget.h"
#include "ui_widget.h"
#include <QList>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QFileInfo>
#include <QWebFrame>

/*********************************************************************************************/
/*初始化*/
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    //初始化标题
    setWindowTitle("协同导航软件v1");
    //初始化地图 需要使用绝对路径
    //QFileInfo xmlinfo("qrc:/map/map.html");//设置文件路径:
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                this, SLOT(addObjectToJs()));//连接JS
    ui->webView->load(QUrl("qrc:/map/map.html"));
    //ui->webView->load(QUrl::fromLocalFile(xmlinfo.absoluteFilePath()));//打开路径下的文件
    //初始化端口
    serialport = new QSerialPort(this);
    portFind();
    //初始化按键
    initButton();
}

//析构函数
Widget::~Widget()
{
    serialport->clear();
    serialport->close();
    delete ui;
}

/*********************************************************************************************/
/*初始化按键*/

void Widget::initButton()
{
    ui->send_button->setEnabled(false);
    ui->close_port->setEnabled(true);
    ui->clear_send_button->setEnabled(false);
    ui->clear_receive_button->setEnabled(false);
    ui->send_state->setEnabled(false);
    ui->receive_state->setEnabled(false);
    ui->fore_button->setEnabled(false);
    ui->back_button->setEnabled(false);
    ui->left_button->setEnabled(false);
    ui->right_button->setEnabled(false);
    ui->up_button->setEnabled(false);
    ui->down_button->setEnabled(false);
    ui->location_button->setEnabled(false);
}

/*开启按键*/
void Widget::openButton()
{
    ui->send_button->setEnabled(true);
    ui->close_port->setEnabled(true);
    ui->open_port->setEnabled(false);
    ui->clear_send_button->setEnabled(true);
    ui->clear_receive_button->setEnabled(true);
    ui->send_state->setEnabled(true);
    ui->receive_state->setEnabled(true);
    ui->com->setEnabled(false);
    ui->baud->setEnabled(false);
    ui->databit->setEnabled(false);
    ui->checkbit->setEnabled(false);
    ui->stopbit->setEnabled(false);
    ui->fore_button->setEnabled(true);
    ui->back_button->setEnabled(true);
    ui->left_button->setEnabled(true);
    ui->right_button->setEnabled(true);
    ui->up_button->setEnabled(true);
    ui->down_button->setEnabled(true);
    ui->location_button->setEnabled(true);
}

/*关闭按键*/
void Widget::closeButton()
{
    ui->send_button->setEnabled(false);
    ui->open_port->setEnabled(true);
    ui->close_port->setEnabled(true);
    ui->clear_send_button->setEnabled(false);
    ui->clear_receive_button->setEnabled(false);
    ui->send_state->setEnabled(false);
    ui->receive_state->setEnabled(false);
    ui->fore_button->setEnabled(false);
    ui->back_button->setEnabled(false);
    ui->left_button->setEnabled(false);
    ui->right_button->setEnabled(false);
    ui->up_button->setEnabled(false);
    ui->down_button->setEnabled(false);
    ui->location_button->setEnabled(false);
    ui->com->setEnabled(true);
    ui->baud->setEnabled(true);
    ui->databit->setEnabled(true);
    ui->checkbit->setEnabled(true);
    ui->stopbit->setEnabled(true);
    ui->com->clear();
}

/*********************************************************************************************/
/*自动查询串口*/
void Widget::portFind()
{
    //检查可用的端口，添加到下拉框以供选择
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial_num;
        serial_num.setPort(info);
        if(serial_num.open(QIODevice::ReadWrite))
        {
            ui->com->addItem(serial_num.portName());
            serial_num.close();
        }
    }
}

/*********************************************************************************************/
/*打开串口*/
void Widget::on_open_port_clicked()
{
    //初始化界面并初始化端口
    update();
    //初始化串口的各项参数
    serialport->setPortName(ui->com->currentText());
    //如果成功打开串口则允许设置参数，否则报错提示
    if(serialport->open(QIODevice::ReadWrite))
    {
        //设置波特率
        serialport->setBaudRate(ui->baud->currentText().toInt());
        //设置数据位
        switch (ui->databit->currentIndex()) {
        case 5:
            serialport->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serialport->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serialport->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            serialport->setDataBits(QSerialPort::Data8);
            break;
        default:
            break;
        }
        //设置奇偶校验位
        switch (ui->checkbit->currentIndex()) {
        case 0:
            serialport->setParity(QSerialPort::NoParity);
            break;
        default:
            break;
        }
        //设置停止位
        switch (ui->stopbit->currentIndex()) {
        case 1:
            serialport->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serialport->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            break;
        }
        //设置流控制
        serialport->setFlowControl(QSerialPort::NoFlowControl);
        //串口设置完成，监听数据
        //connect(serialport, &QSerialPort::readyRead, this, &Widget::read_data);
        connect(serialport, &QSerialPort::readyRead, this, &Widget::readData);
        //重置按键状态
        openButton();
    }
    else
    {QMessageBox::information(this, tr("Warning"), tr("Cannot open this serialport!"), QMessageBox::Ok);}
}
/*关闭串口*/
void Widget::on_close_port_clicked()
{
    //关闭串口，重置按键，重查串口，清空文本框
    serialport->clear();
    serialport->close();
    closeButton();
    portFind();
    ui->Send_text_win->clear();
    ui->Receive_text_win->clear();
}
void Widget::convertGpsData(QString srcData, QString &destData,int pos)
{
    int dotPos = srcData.indexOf(".");
    QString min=srcData.right(srcData.size()-(dotPos-2));
    double fMin = min.toFloat();
    double fD = (double)(srcData.left(pos).toInt()+(fMin/60));
    destData = QString("%1").arg(QString::number(fD, 'f', 9)); //取到小数点后9位
//    qDebug()<<"min: "<<(fMin/60)<<",fD: "<<fD;
}
/********接收并处理串口中的数据*************/
void Widget::readData()//创建一个readData对象
{
    QByteArray Data = serialport->readAll();//读取所有数据存入QByteArray类的对象Data里
    if(!Data.isEmpty()) //如果Data不为空
    {
        ui->Receive_text_win->append(Data);//大框里显示所有接收数据
        qDebug()<<"data:"<<Data;
        //GPS数据处理
        if(Data.contains("$GPRMC"))                //如果这串数据中包含$GPRMC（具体百度GPS帧格式，了解接收数据的帧格式）
        {
            Data.remove(0,Data.indexOf("$GPRMC")); //删除数据起始到$GPRMC之间的数据
            if(Data.contains("*"))                     //如果剩下的字符中包含*
            {
                GPS_RMC = Data.left(Data.indexOf("*"));
                QString gpsStr=GPS_RMC.data();
                QStringList strList = gpsStr.split(',');
                QString lat_str;      //纬度
                QString lon_str;     //经度
                lat_str = strList.at(3);
                lon_str= strList.at(5);
                if(strList.at(2)=="A")
                {
                    convertGpsData(strList.at(3),lat_str,2);
                    convertGpsData(strList.at(5),lon_str,3);
                    QString fun=QString("doLocal(\"%1\",\"%2\");").arg(lon_str).arg(lat_str);
                    qDebug()<<"fun: "<<fun ;
                    ui->webView->page()->mainFrame()->evaluateJavaScript(fun);
                }
                strList.clear();
                GPS_RMC.clear();
            }
        }
    }
    Data.clear();
}

void Widget::addObjectToJs()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("Widget", this);
}

/*发送数据*/
void Widget::on_send_button_clicked()
{
    //qDebug()<<"send";
    //字符串或者十六进制发送
    QString str = ui->Send_text_win->toPlainText();
    if(!str.isEmpty())
    {
        if(ui->send_state->currentText() == "string")
        {
            serialport->write(str.toLatin1());
        }
        else
        {
            int num = str.toInt();
            str = str.setNum(num, 16);
            serialport->write(str.toLatin1());
        }
     }
}

/*********************************************************************************************/
/*清空接收窗口*/
void Widget::on_clear_receive_button_clicked()
{
    ui->Receive_text_win->clear();
}
/*清空发送窗口*/
void Widget::on_clear_send_button_clicked()
{
    ui->Send_text_win->clear();
}


/**********定位************/
//需要先打开串口才能使用
void Widget::on_location_button_clicked()
{
    QString str = "@DTU:0000:GPSINFO";
    serialport->write(str.toLatin1());
}


/*********************************************************************************************/
/*控制*/
//需要先打开串口才能使用
//上升按钮
void Widget::on_up_button_clicked()
{
    QString str = "up";
    serialport->write(str.toLatin1());

}
//下降按钮
void Widget::on_down_button_clicked()
{
    mysleep(600);
    QString str = "down";
    serialport->write(str.toLatin1());
}
//前进按钮
void Widget::on_fore_button_clicked()
{
    QString str = "fore";
    serialport->write(str.toLatin1());
    carStop();
}
//后退按钮
void Widget::on_back_button_clicked()
{
    QString str = "back";
    serialport->write(str.toLatin1());
    carStop();
}
//右转按钮
void Widget::on_right_button_clicked()
{
    QString str = "right";
    serialport->write(str.toLatin1());
    carStop();
}
//左转按钮
void Widget::on_left_button_clicked()
{
    QString str = "left";
    serialport->write(str.toLatin1());
    carStop();
}

//定时停止
void Widget::carStop()
{
    mysleep(600);
    QString str = "stop";
    serialport->write(str.toLatin1());
}

/*延时函数*/
void Widget::mysleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}




