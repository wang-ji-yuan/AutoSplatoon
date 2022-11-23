#ifndef CUSTOMQGRAPHICSVIEW_H
#define CUSTOMQGRAPHICSVIEW_H

#include <QMainWindow>
#include <QLabel>
#include  <QGraphicsView>
#include  <QPoint>
#include <QMouseEvent>
class CustomQGraphicsView : public QGraphicsView
{
public:
    CustomQGraphicsView();
    CustomQGraphicsView(QWidget *parent);
    QLabel *scenePointDispaly = new QLabel;
    QPointF scenePoint;

protected:
    void mouseMoveEvent(QMouseEvent *event);    //鼠标移动事件

private:

};



#endif // CUSTOMQGRAPHICSVIEW_H
