#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "mygraphicsview.h"
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MyWidget; }
QT_END_NAMESPACE

//显示模式
enum DisplayModeEnum
{
    TOPLEFT = 0,            //左上角顶点
    XHorizontalMiddle,      //X水平轴居中
    YVerticalMiddle         //Y垂直轴居中
};

class MyWidget : public QWidget
{
    Q_OBJECT

public:
    MyWidget(QWidget *parent = nullptr);
    ~MyWidget();

private slots:
    void on_pushBtnLoadImage_clicked();
    void on_pushButton_clicked();
    void ReleasedResourceSlot();

private:
    qreal GetDefaultRatio(const QPixmap &pix,DisplayModeEnum &displayMode);

private:
    Ui::MyWidget *ui;
    QGraphicsScene *m_pScene;   //场景
    MyGraphicsView *m_pView;    //视口

    qreal m_qrShrinkedRatio;        //视口缩放比例
    qreal m_qrDefaultShrinkedRatio; //图形项默认缩放比例
    int m_nViewWidth;
    int m_nViewHeight;
};
#endif // MYWIDGET_H
