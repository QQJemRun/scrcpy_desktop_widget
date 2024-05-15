#include <QGridLayout>
#include <QDebug>
#include <QMessageBox>
#include <QListWidget>
#include <QFile>
#include "widget.h"
#include <QPainter>
#include <QScreen>
#include <QGuiApplication>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    this->setAttribute(Qt::WA_TranslucentBackground );
    widget = new QWidget();
    this->effect_shadow = new QGraphicsDropShadowEffect;
    this->effect_shadow->setOffset(0,0);
    this->effect_shadow->setBlurRadius(30);
    this->effect_shadow->setColor(Qt::gray);
    this->setGraphicsEffect(this->effect_shadow);
    widget->setWindowFlags(Qt::WindowCloseButtonHint|Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    widget->setFixedSize(250,150);
    widget->setAttribute(Qt::WA_TranslucentBackground );
    shell = new QProcess(this);
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Widget::flush_pixmap);
    connect(shell , &QProcess::readyReadStandardError , this , [=]()
            {
                QFile *file = new QFile( "catgirl.log");
                if(!file->open(QIODevice::ReadWrite | QIODevice::Append))
                {
                    return;
                }
                QTextStream stream = QTextStream(file);
                stream<<shell->readAllStandardError()<<Qt::endl;
            });
    timer->start(1000);
    QGridLayout *gl = new QGridLayout();
    this->setLayout(gl);
    screen = QGuiApplication::primaryScreen();
    flush_pixmap();
    this->move(screen->availableGeometry().width()-this->width()-10,screen->availableGeometry().height()-this->height()-60);
}

Widget::~Widget()
{
    shell->close();
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    //只允许左键拖动   持续的动作
    if(event->buttons()&Qt::LeftButton)  //buttons处理的是长事件，button处理的是短暂的事件
    {
        move((event->globalPosition()-m_pt).toPoint());
    }
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(!widget->isHidden())
    {
        widget->hide();
        return;
    }
    if(shell->isOpen())
        shell->readAll();
    if(event->button()&Qt::LeftButton)
    {
        connect(shell , &QProcess::readyReadStandardOutput , this ,&Widget::devices_process,Qt::SingleShotConnection);
        if(!shell->isOpen())
        {
            shell->start("bash");
            if(!shell->waitForStarted())
            {
                qDebug()<<"error";
            }
        }
        shell->write("adb devices\n");
    }
}


void Widget::mousePressEvent(QMouseEvent *ev)
{
    // 如果鼠标左键按下  单击
    if(ev->button()&Qt::LeftButton)
    {
        //求差值=鼠标当前位置-窗口左上角点
        m_pt=ev->globalPosition()-this->geometry().topLeft();  //geometry()是矩形窗口，topLeft()是左上角的信息。
    }
}

void Widget::flush_pixmap()
{
    pix.load(":/1.png");
    this->resize(pix.size());
    this->setMask(pix.mask());
    pix.load(QString(":/%1.png").arg(x));
    x=x%6+1;
    this->update();
}

void Widget::devices_process()
{
    for (auto w: widget->children())
       delete w;

    QStringList devices = QString(shell->readAllStandardOutput()).split("\n");
    qDebug()<<devices;
    devices.removeFirst();
    devices.removeLast();
    if(devices.isEmpty())
        return;
    QListWidget *devices_list = new QListWidget(widget);
    devices_list->setFixedSize(widget->size());
    devices_list->setStyleSheet("background-color: rgba(165, 58, 224, 159);"
                                                    "gridline-color: rgba(255, 255, 255,50);"
                                                    "border-color: rgba(255, 255, 255,50);"
                                                    "selection-color: rgba(255, 255, 255,50);"
                                                    "selection-background-color: rgba(255, 255, 255,50);"
                                                    "alternate-background-color: rgba(255, 255, 255,50);"
                                                    "border-radius:10px;");
//    wl->addWidget(devices_list);
    devices_list->addItems(devices);
//    widget->setLayout(wl);
    widget->adjustSize();
    if(this->geometry().topLeft().rx()<960)
    {
        widget->move(this->geometry().topLeft()+QPoint(100,100));
    }
    else
    {
        widget->move(this->geometry().topLeft()-QPoint(250,-100));
    }
    widget->show();
    connect(devices_list,&QListWidget::doubleClicked,this,[=](QModelIndex index)
            {
        disconnect(shell , &QProcess::readyReadStandardOutput , this ,&Widget::devices_process);
        if(!shell->isOpen())
        {
            shell->start("bash");
            if(!shell->waitForStarted())
            {
                qDebug()<<"error";
            }
        }
        devices_list->item(index.column())->text().split("\t").first();

        shell->write(QString("scrcpy -s %1 --turn-screen-off\n").arg(devices_list->item(index.column())->text().split("\t").first()).toUtf8());
        widget->hide();
    });
}

void Widget::paintEvent(QPaintEvent *event)
{
    event->accept();
    QPainter *painter =new QPainter(this);
    painter->drawPixmap(0,0,this->pix);
    delete painter;
}

