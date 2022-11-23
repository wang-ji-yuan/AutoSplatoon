#include "customqgraphicsview.h"
#include "qevent.h"

CustomQGraphicsView::CustomQGraphicsView()
{

}

CustomQGraphicsView::CustomQGraphicsView(QWidget *parent = nullptr)
{

}


//鼠标移动事件
void CustomQGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    //QGraphicsView坐标
    QPoint viewPoint = event->pos();

    //QGraphicsScene坐标
    scenePoint = mapToScene(viewPoint);
    setMouseTracking(true);
    scenePointDispaly->setText("("+QString::number(scenePoint.x())+","+QString::number(scenePoint.y())+")");

}
