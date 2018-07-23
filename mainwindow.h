#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "item.h"
#include <QMainWindow>
#include<QTimer>
#include <QLabel>
#include<QSound>
namespace Ui {
class MainWindow;
}
#define item_width      30//每一个小格子的宽度
#define item_height     30//每一个小格子的高度

#define frame_x        0 //边框参数
#define frame_y         60//边框参数

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);//构造函数
    ~MainWindow();                      //析构函数

    void initializeitems();                   //随机初始化小格子中雷点位置
    void  releaseitems();                //清除原有小格子中的信息

    void restart();                     //重新开一局小游戏
    void lose();                    //表示点到雷则游戏失败
    bool iffindall();                     //判断是否找到所有雷
    void paintEvent(QPaintEvent *);     //绘制界面
    int rMine;                          //剩余雷数


protected:
    void mousePressEvent(QMouseEvent *);//鼠标点击事件

private slots:
    void OnMenuNewGame();//重新开始一局小游戏
    void OnMenuLevel1(); //初级
    void OnMenuLevel2(); //中级
    void OnMenuLevel3(); //高级
    void on_sectime();   //计时器


private:
    void drawchessboard();//绘制棋盘
    void drawitems();     //绘制小格子
    void drawitem(QPainter& painter,Item* pItem);//绘制游戏界面

    void opentheblank(QPoint pt);			//点击相邻雷数为0的小格子时，递归查找相邻的空白格子，以及空白格子附近的带数字的小格子
    bool ifinarea(QPoint pt);		//判断鼠标点击坐标是否超过游戏区域


private:
    Ui::MainWindow *ui;
    QPixmap m_carrot;				//胡萝卜图片
    QPixmap m_worm;                //毛毛虫图片
    QPixmap m_thetimer;                   //计时器
    QPixmap m_central;                  //中上方表情包

    int m_rows;						//小格子行数
    int m_columes;                     //小格子列数
    int m_totalmine;					//总共雷数
    QVector<QPoint> m_Mines;			//剩余雷点
    QVector<QVector<Item*>> m_items;	//所有小格子
    bool m_ifgamefail;//是否是游戏失败，失败了需要显示雷
    QTimer *rumtime;//计时器
    int set_x,set_y;//框架参数
};

#endif // MAINWINDOW_H

