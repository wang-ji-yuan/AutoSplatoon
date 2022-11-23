#ifndef CUSTOMGRAPHICSSCENE_H
#define CUSTOMGRAPHICSSCENE_H

#include <QGraphicsScene>

class CustomGraphicsScene : public QGraphicsScene
{
public:
    CustomGraphicsScene();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
//    QGraphicsItem *m_pItemSelected; //鼠标选中的QGraphicsItem
//    QPointF m_Offset; //鼠标移动的偏移量
    QPointF m_Pressed; //鼠标按下的位置点
//    QPointF m_ItemPos; //鼠标选中的QGraphicsItem在场景中的位置
    bool m_bPressed; //鼠标是否按下
};

#endif // CUSTOMGRAPHICSSCENE_H
