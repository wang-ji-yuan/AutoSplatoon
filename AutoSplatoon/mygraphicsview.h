#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H
#include <QGraphicsView>
#include <vector>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyGraphicsView(QWidget *parent = nullptr);
public:
    qreal GetScaledValue() const   //获取最新的放大倍数
    {
        return m_qrScaledNum;
    }
    void SetScaledDefaultValue()
    {
        this->m_qrScaledNum = 1;
    }
    void setQMouseEventUsable(bool flag)
    {
        this->mouseUsable=flag;
    }
//signals:
//    void mouseRelease(bool);

protected:
    void wheelEvent(QWheelEvent *ev);   //重写graphicsView的滚轮事件
//    void mousePressEvent(QMouseEvent *ev);   //重写鼠标按下事件
//    void mouseReleaseEvent(QMouseEvent *ev);//重写鼠标释放事件

private:
    qreal m_qrScaledNum;   //视口缩放倍数
    bool mouseUsable=true;
};
#endif // MYGRAPHICSVIEW_H

