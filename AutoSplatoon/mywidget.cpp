#include "mywidget.h"
#include "ui_mywidget.h"
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>

#define cout qDebug() << "["<< __FILE__ <<":" << __LINE__<<"]"
#define VIEW_MAX_WIDTH 800      //视口最大宽度
#define VIEW_MAX_HEIGHT 600     //视口最大高度


MyWidget::MyWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyWidget)
{
    ui->setupUi(this);

    //创建视图
    this->m_pView = new MyGraphicsView(this);
    m_pView->resize(VIEW_MAX_WIDTH,VIEW_MAX_HEIGHT);
    this->m_nViewWidth = m_pView->width();
    this->m_nViewHeight = m_pView->height();

    //创建场景
    this->m_pScene = new QGraphicsScene(this);
    //设置场景大小和左上角点坐标
    this->m_pScene->setSceneRect(0,0,VIEW_MAX_WIDTH,VIEW_MAX_HEIGHT);

    //窗口关闭释放资源
    connect(this,&MyWidget::destroyed,this,&MyWidget::ReleasedResourceSlot);
}

MyWidget::~MyWidget()
{
    if(nullptr != m_pView)
    {
        delete m_pView;
        m_pView = nullptr;
    }

    if(nullptr != m_pScene)
    {
        delete m_pScene;
        m_pScene = nullptr;
    }

    delete ui;
}

void MyWidget::on_pushBtnLoadImage_clicked()
{
     //位置还原
     on_pushButton_clicked();
     //清空场景中的图形项
     m_pScene->clear();

     //加载图片
     QString strFilePath = QFileDialog::getOpenFileName(this,tr("Load a Image!"),"C:/Users/zhengfei6/Desktop/testImage",tr("Images(*.png *.jpg *.bmp)"));
     if(NULL == strFilePath)
     {
         cout << "FilePath is Null";
         return;
     }

     //加载图片
     QPixmap pix;
     pix.load(strFilePath);
     //获取缩放比例
     DisplayModeEnum displayMode = DisplayModeEnum::TOPLEFT;  //默认左上顶点显示
     this->m_qrDefaultShrinkedRatio = GetDefaultRatio(pix,displayMode);

     QGraphicsPixmapItem *pPixItem = new QGraphicsPixmapItem(pix);
     pPixItem->setScale(1/this->m_qrDefaultShrinkedRatio);
     pPixItem->setFlag(QGraphicsItem::ItemIsMovable,true);

     QGraphicsRectItem *pRectItem = new QGraphicsRectItem(QRectF(0,0,20,20));
     pRectItem->setFlag(QGraphicsItem::ItemIsMovable,true);

     //图形项显示模式
     switch (displayMode)
     {
     case TOPLEFT:   //左上顶点显示
         pPixItem->setPos(0,0);
         break;
     case XHorizontalMiddle:    //水平居中显示
     {
         qreal qrXPos = ((qreal)VIEW_MAX_WIDTH-(qreal)pix.width()/m_qrDefaultShrinkedRatio)/2;
         cout << "qrXPox = "<<qrXPos;
         pPixItem->setPos(qrXPos,0);
     }
         break;
     case YVerticalMiddle:      //垂直居中显示
     {
         qreal qrYPos = ((qreal)VIEW_MAX_HEIGHT-(qreal)pPixItem->pixmap().height()/m_qrDefaultShrinkedRatio)/2;
         cout << "qrYPos = " << qrYPos;
         pPixItem->setPos(0,qrYPos);
     }
         break;
     default:
         break;
     }

     //添加图形项
     m_pScene->addItem(pPixItem);
     m_pScene->addItem(pRectItem);

     //设置场景
     m_pView->setScene(m_pScene);
     m_pView->show();
}

qreal MyWidget::GetDefaultRatio(const QPixmap &pix,DisplayModeEnum &displayMode)
{
    qreal dResRatio = 0;
    qreal qrWidthRatio = (qreal)pix.width()/(qreal)VIEW_MAX_WIDTH;
    qreal qrHeightRatio = (qreal)pix.height()/(qreal)VIEW_MAX_HEIGHT;
    // cout << "qrWidthRatio = "<< qrWidthRatio;
    qreal nMaxTmp = qMax(qrWidthRatio,qrHeightRatio);  //取宽宽比和高高比的大者

    //  cout << "nMaxTmp = "<< nMaxTmp;
    if(qrWidthRatio == nMaxTmp)
    {
        dResRatio = qrWidthRatio;
        displayMode = DisplayModeEnum::YVerticalMiddle;
    }
    else
    {
        dResRatio = qrHeightRatio;
        displayMode = DisplayModeEnum::XHorizontalMiddle;
    }
    //cout << "dResRatio = "<< dResRatio;
    return dResRatio;
}

//视口还原为原来的缩放比例
void MyWidget::on_pushButton_clicked()
{
    //获取view的缩放倍数
    qreal qrTmp = m_pView->GetScaledValue();
    m_pView->scale(1/qrTmp,1/qrTmp);

    //还原后，更新下view中的放大倍数为1
    m_pView->SetScaledDefaultValue();

    //图形项还原位置
    QList<QGraphicsItem*> pListItems = m_pScene->items();
    foreach(QGraphicsItem* pItem , pListItems)
    {
        pItem->setPos(0,0);
    }
}
void MyWidget::ReleasedResourceSlot()
{
#if 1
    if(nullptr != m_pView)
    {
        delete m_pView;
        m_pView = nullptr;
    }

    if(nullptr != m_pScene)
    {
        delete m_pScene;
        m_pScene = nullptr;
    }
#endif
    cout <<"close the widget";
}
