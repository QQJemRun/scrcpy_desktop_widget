#ifndef WIDGET_H
#define WIDGET_H
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QMouseEvent>
#include <QProcess>
#include <QWidget>
#include <QGraphicsDropShadowEffect>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    int x = 1;
    QPixmap pix;
    void flush_pixmap();
    QTimer *timer;
    QPointF m_pt;
    QProcess *shell;
    QWidget *widget;
    QGraphicsDropShadowEffect *effect_shadow;
    QScreen *screen;
    void devices_process();
};
#endif // WIDGET_H
