//每一个小格子上的情况
#ifndef ITEM_H
#define ITEM_H

#include <QPoint>
#include <QLabel>

class Item
{
public:
    Item();
    Item(QPoint pos);

    QPoint m_pos;	//小格子的位置

    bool m_bmine;	//小格子中是否是雷
    bool m_bMarked;	//小格子是否已标记为雷

    int m_nNumber;	//小格子中的数字
    bool m_bOpen;	//小格子是否已打开，且非雷
    int timer=0;    //计时器
    int winf=0;     //框架
    QPixmap frame;//框架中间图片
};

#endif // ITEM_H
