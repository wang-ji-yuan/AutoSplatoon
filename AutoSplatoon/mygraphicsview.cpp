#include "mygraphicsview.h"
#include <QWheelEvent>
#include <QGraphicsItem>
#include <QDebug>
//#define cout qDebug() << "["<< __FILE__ <<":" << __LINE__<<"]"

MyGraphicsView::MyGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    this->setMouseTracking(true);   //跟踪鼠标位置
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //隐藏水平条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);     //隐藏竖条
    m_qrScaledNum = 1; //初始化
}

//缩放
void MyGraphicsView::wheelEvent(QWheelEvent *ev)
{
        if(this->mouseUsable)
      {
        qreal qrTmp = 1.0;

        if(ev->delta() > 0)
        {
            qrTmp = 1.2;
            this->scale(qrTmp,qrTmp);
        }
        else
        {
            qrTmp = 1.0/1.2;
            this->scale(qrTmp,qrTmp);
        }
        m_qrScaledNum *= qrTmp;  //保存放大倍数
    }

}

//void MyGraphicsView::mousePressEvent(QMouseEvent *ev)
//{

//    QGraphicsView::mousePressEvent(ev);
//}
//void MyGraphicsView::mouseReleaseEvent(QMouseEvent *ev){
//    //代表鼠标右键按下
//    if(ev->button()==Qt::LeftButton){
//       emit mouseRelease(true);
//    }
//}
