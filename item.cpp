#include "item.h"

Item::Item()
{
    m_pos = QPoint(-1,-1);

    m_nNumber = 0;    //数字为零
    m_bmine = false;//表示是雷
    m_bMarked = false;//表示没有被标记为雷
    m_bOpen = false;  //未被打开
}

Item::Item(QPoint pos)//所点到的格子
{
    m_pos = pos;//赋给位置参数

    m_nNumber = 0;
    m_bmine = false;
    m_bMarked = false;
    m_bOpen = false;
}
