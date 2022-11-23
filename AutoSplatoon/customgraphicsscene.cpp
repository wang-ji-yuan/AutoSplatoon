#include "customgraphicsscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>

CustomGraphicsScene::CustomGraphicsScene()
    : m_bPressed(false)
{
}

void CustomGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        m_bPressed = true;
//        m_Pressed = event->scenePos(); //记录此时鼠标在场景中位置点

//        m_pItemSelected = nullptr;
//        foreach(QGraphicsItem *item, items(event->scenePos()))//循环查找当前鼠标位置点下所有的Item
//        {
//            if(item->type() == QGraphicsProxyWidget::Type)//判断此Item是否为添加的ActorWidget创建的QGraphicsProxyWidget
//            {
//                m_bPressed = true;
//                m_Pressed = event->scenePos(); //记录此时鼠标在场景中位置点
//                m_pItemSelected = item; //选中的Item

//                QGraphicsProxyWidget *pProxyWidget = qgraphicsitem_cast<QGraphicsProxyWidget*>(item);
//                m_ItemPos = pProxyWidget->scenePos(); //QGraphicsProxyWidget的位置点

//                break;
//            }
//        }
    }
}

void CustomGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
    m_Pressed = event->scenePos(); //记录此时鼠标在场景中位置点
//    if(m_pItemSelected != nullptr)
//    {
//        if(m_bPressed)
//        {
//            m_Offset = event->scenePos() - m_Pressed; //鼠标移动的偏移量
//            m_pItemSelected->setPos(m_ItemPos + m_Offset); //结合鼠标按下时Item的位置点，重新设置Item的位置点
//        }
//    }
}

void CustomGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseReleaseEvent(event);
    m_bPressed = false;
//    if(m_pItemSelected != nullptr)
//    {
//        m_pItemSelected = nullptr; //鼠标释放时将选中的Item设为空
//    }
}
