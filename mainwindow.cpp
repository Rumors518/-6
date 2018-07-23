#include"mainwindow.h"
#include"ui_mainwindow.h"
#include<QPaintEvent>
#include<QPainter>
#include<QTime>
#include<QMessageBox>
#include<QLayout>
#include<windows.h>
#include<QVBoxLayout>
#include<QColor>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->hide(); //隐藏工具栏
    rumtime=new QTimer(this);//连接计时器

    m_carrot = QPixmap(":/new/image/flag.png");	//胡萝卜
    m_worm = QPixmap(":/new/image/bomb.png");	//虫子
    m_thetimer = QPixmap(":/new/image/number.bmp");   //计时器
    m_central=QPixmap(":new/image/ing.png");        //重新开始图片

    m_rows = 16;     //默认难度为中级难度
    m_columes = 16;
    m_totalmine = 40;
    set_x=5; //框架参数
    set_y=30;

    connect(ui->menu,SIGNAL(aboutToShow()),this,SLOT(OnMenuNewGame()));           //新游戏槽
    connect(ui->menu_2,SIGNAL(aboutToShow()),this,SLOT(OnMenu_Settings()));       //难易槽
    connect(ui->action_9_9_10, SIGNAL(triggered()), this, SLOT(OnMenuLevel1()));  //初级槽
    connect(ui->action_16_16_40, SIGNAL(triggered()), this, SLOT(OnMenuLevel2()));//中级槽
    connect(ui->action_25_25_99, SIGNAL(triggered()), this, SLOT(OnMenuLevel3()));//高级槽
    connect( rumtime,SIGNAL(timeout()), this, SLOT(on_sectime()) );               //计时器槽

    //欢迎图片
    setWindowTitle (tr("史上最萌扫雷小游戏"));//游戏名称
    resize (600, 450);//窗口大小
    Sleep (1000);     //使主窗口程序在初始化时休眠几秒钟，显示欢迎图片
    QSound *sound = new QSound(":new/image/恋爱循环.wav");//背景音乐
    sound->play();
    sound->setLoops(100);
    restart();//开始游戏
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeitems()//随机初始化小格子中雷点位置
{
    //随机初始化雷
    m_Mines.clear();//清除函数
    rMine=m_totalmine;//把设定的每一个难度的雷数赋值给现在的雷数
    for(int i = 0; i<m_totalmine; i++)
    {
        qsrand(QTime::currentTime().msec());//随机数
        int x = qrand()%m_columes;
        int y = qrand()%m_rows;
        while(m_Mines.contains(QPoint(x,y)))
        {
            x = qrand()%m_columes;
            y = qrand()%m_rows;
        }
        m_Mines.append(QPoint(x,y));
    }
    for(int i=0; i<m_columes; i++)//建立2维数组保存所有小格子的位置
    {
        QVector<Item*> rowItems;
        for(int j=0; j<m_rows; j++)
        {
            QPoint pos = QPoint(i,j);
            Item* pItem = new Item(pos);
            if(m_Mines.contains(pos))   //如果该位置是雷
            {
                pItem->m_bmine = true;
            }
            rowItems.append(pItem);
        }
        m_items.append(rowItems);
    }
    for(int i=0; i<m_columes; i++)//计算雷附近格子的数字
    {
        for(int j=0; j<m_rows; j++)
        {
            if (m_items[i][j]->m_bmine)//跳过是雷的小格子
            {
                continue;
            }
            int nCountMines = 0;
            for (int m=-1;m<=1;m++)//求每个点附近的8个点的是雷的总数
            {
                for (int n=-1; n<=1;n++)
                {
                    if (m==0 && n==0)
                    {
                        continue;
                    }
                    QPoint ptNew = QPoint(i+m,j+n);
                    if (!ifinarea(ptNew))
                    {
                        continue;
                    }

                    if (m_items[i+m][j+n]->m_bmine)
                    {
                        nCountMines++;
                    }
                }
            }
            m_items[i][j]->m_nNumber = nCountMines;//记录
        }
    }
}

void MainWindow::   releaseitems()//清除原有小格子中的信息
{
    for (int i=0; i<m_items.size(); i++)
    {
        for (int j=0;j<m_items[i].size(); j++)
        {
            if (m_items[i][j] != NULL)
            {
                delete m_items[i][j];
                m_items[i][j] = NULL;
            }
        }
    }
    m_items.clear();
}

void MainWindow::restart()//开始新游戏
{
    m_ifgamefail = false;
    m_central=QPixmap(":new/image/ing.png");//中间图片
    resize(frame_x*2 + m_columes*item_width  ,frame_y+ m_rows*item_height);
      releaseitems();
    initializeitems();
}

void MainWindow::lose()//游戏失败
{
    m_ifgamefail = true;
    for (int i=0; i<m_items.size(); i++)
    {
        for (int j=0;j<m_items[i].size(); j++)
        {
            if (m_items[i][j]->m_bmine)
            {
                m_items[i][j]->m_bMarked = true;
            }
            else
            {
                m_items[i][j]->m_bMarked = false;
                m_items[i][j]->m_bOpen = true;
            }
        }
    }
}

bool MainWindow::iffindall()//判断是否找完
{
    bool bFindAll = true;
    //遍历二维数组 QVector<QVector<Item*>> m_items
    for (int i=0; i<m_items.size(); i++)
    {
        for (int j=0;j<m_items[i].size(); j++)
        {
            //检查是否每一个雷都被标记或找出
            Item* pItem = m_items[i][j];
            if (pItem->m_bmine)
            {
                if (!pItem->m_bMarked)
                {
                    bFindAll = false;
                }
            }
            else
            {
                if (!pItem->m_bOpen)
                {
                    bFindAll = false;
                }
            }
        }
    }
    return bFindAll;
}

void MainWindow::paintEvent(QPaintEvent *e)//绘制界面
{
    QPixmap thetimer(":new/image/item1.bmp");//计时器数字
    QPixmap frame(":new/image/item3.bmp");//框架
    drawchessboard();
    drawitems();
    QPainter painter(this);
    QPoint pt;
    Item* pItem = m_items[pt.x()][pt.y()];
    int rm=rMine;
    int rt=pItem->timer;
    //绘制框架
    painter.drawPixmap( set_x-6,set_y-10, frame, 0, 0, 70, 40);
    painter.drawPixmap(m_rows* 15-24+set_x, set_y-10, frame, 80, 0, 40, 40);
    painter.drawPixmap(m_rows* 30 -73+set_x, set_y-10, frame, 130, 0, 70, 40);
    painter.drawPixmap(64+set_x, set_y-10, m_rows* 15 - 88, 40, frame, 70, 0, 10, 40);
    painter.drawPixmap(m_rows* 15 + 16+set_x, set_y-10,m_rows * 15- 88, 40, frame, 70, 0, 10, 40);
    painter.drawPixmap(m_rows* 15 -16+set_x,set_y-3, m_central, pItem->winf * 24, 0, 24, 24);
    //绘制剩余雷数
    if (rm < 0) rm = 0;
    painter.drawPixmap(set_x, set_y-5, thetimer, rm / 100 * 20, 0, 20, 28);
    if (rm >= 100) rm %= 100;
    painter.drawPixmap(20+set_x, set_y-5, thetimer, rm / 10 * 20, 0, 20, 28);
    painter.drawPixmap(40+set_x,set_y-5, thetimer, rm % 10 * 20, 0, 20, 28);
    //绘制扫雷时间
    if (rt >= 1000) rt %= 1000;
    painter.drawPixmap(m_rows*30-68+set_x, set_y-5, thetimer, rt/100*20,0 ,20 , 28);
    if (rt >= 100) rt  %= 100;
    painter.drawPixmap(m_rows*30-48+set_x, set_y-5, thetimer, rt/10*20 ,0,20, 28);
    painter.drawPixmap(m_rows*30-28+set_x, set_y-5, thetimer, rt%10*20,0, 20, 28);
    update();
}

void MainWindow::mousePressEvent(QMouseEvent * e)//鼠标点击事件
{
    //得到鼠标处的格子坐标
    QPoint pt,pp;
    pt.setX( (e->pos().x() - frame_x) / item_width);
    pt.setY( (e->pos().y() - frame_y) / item_height);
    pp.setX(e->pos().x() - frame_x) ;
    pp.setY(e->pos().y() - frame_y) ;
    //点击中上方的表情包，重启游戏
        if (pp.y()<0)
        {   if(m_columes%2)
            {if(pt.y()>=0&&pt.x()== m_columes/2)
               {
                    if (rumtime->isActive() )rumtime->stop();
                    restart();
                }
            }
            else
            {
                if(pt.y()>=0&&pt.x()== m_columes/2||pt.x()== m_columes/2-1)
                {
                     if (rumtime->isActive() )rumtime->stop();
                     restart();
                 }
             }
            return;
        }
    //如果点不在游戏区域内
    if (!ifinarea(pt))
    {
        return;
    }
    //获取所点击矩形元素
    Item*pItem = m_items[pt.x()][pt.y()];
    //左键打开小格子，右键插胡萝卜标记
    if(e->button()==Qt::LeftButton)
    {
        //未处理的小格子
        if(!pItem->m_bMarked && !pItem->m_bOpen)
        {
            //如果是雷，就结束游戏
            if (pItem->m_bmine)
            {
                if (rumtime->isActive() ) rumtime->stop();//停止计时
                lose();
                m_central=QPixmap(":new/image/bad.png");//中间图片
                QMessageBox message(QMessageBox::NoIcon,"Game Over", "<font size='24' color='magenta'>你在南开读傻了？！</font>");
                message.setIconPixmap(QPixmap(":/new/image/lose.png"));
                message.exec();

                return;
            }
            else
            {
                if (!(rumtime->isActive() ))
                    rumtime->start(1000);//开始计时
                pItem->m_bOpen = true;//打开
                if (pItem->m_nNumber == 0)
                {
                    //点到一个是0的小格子，一下打开一大片
                    opentheblank(pt);
                }
                if (iffindall())//如果已找到所有雷
                {
                    if (rumtime->isActive() )rumtime->stop();//停止计时
                    m_central=QPixmap(":new/image/good.png");//改变中间图片
                    QMessageBox message(QMessageBox::NoIcon,"Game Over", "<font size='6' color='magenta'>同样是接受九年义务教育为什么你可以这么优秀?</font>");
                    message.setIconPixmap(QPixmap(":/new/image/win.png"));//输出游戏失败对话框
                    message.exec();
                    return;
                }
            }
        }
    }
    else if(e->button()==Qt::RightButton)
    {
        if (pItem->m_bMarked)//已标记过的小格子，再点击取消标记
        {
            pItem->m_bMarked = false;rMine++;
        }
        else if (!pItem->m_bOpen)
        {
            //没标记也没打开，就是未处理的，就插胡萝卜标记上
            pItem->m_bMarked = true;
            rMine--;
            if (iffindall())
            {
                if (rumtime->isActive() )rumtime->stop();//停止计时
                lose();
                m_central=QPixmap(":new/image/good.png");//中间图片
                QMessageBox message(QMessageBox::NoIcon,"Game Over", "<font size='6' color='magenta'>同样是接受九年义务教育为什么你可以这么优秀?</font>");
                message.setIconPixmap(QPixmap(":/new/image/win.png"));//输出胜利对话框
                message.exec();
                return;
            }
        }
    }

}

void MainWindow::OnMenuNewGame()
{
    restart();
}

void MainWindow::OnMenuLevel1()
{
    m_rows = 9;
    m_columes = 9;
    m_totalmine = 10;
    restart();
}

void MainWindow::OnMenuLevel2()
{
    m_rows = 16;
    m_columes = 16;
    m_totalmine = 40;
    restart();
}

void MainWindow::OnMenuLevel3()
{
    m_rows = 25;
    m_columes = 25;
    m_totalmine = 99;
    restart();
}

void MainWindow::drawchessboard()//画棋盘
{
    QPainter painter(this);
    //painter.setBrush(Qt::lightGray);//改背景的颜色
    //painter.drawRect( 0,0,width(),height());
    painter.drawPixmap(0,0,width(),height(),QPixmap(":/new/image/background.png"));
}

void MainWindow::drawitems()//画小格子
{
    QPainter painter(this);
    painter.setBrush(Qt::lightGray);
    painter.setPen(QPen(QColor(255,85,127),2));//格子边框和数字的颜色

    for(int i=0; i<m_columes; i++)
    {
        for(int j=0; j<m_rows; j++)
        {
            drawitem(painter,m_items[i][j]);
        }
    }
}
void MainWindow::drawitem(QPainter& painter,Item* pItem)//游戏过程中画棋盘
{
    if(pItem->m_bMarked)
    {
        if (m_ifgamefail)
        {
            //游戏结束，显示为雷
            QRect rcSrc(0,0,m_carrot.width(),m_carrot.height());
            QRect rcTarget(frame_x+ pItem->m_pos.x()*item_width + 2,frame_y + pItem->m_pos.y()*item_height + 2,item_width-4,item_height-4);
            painter.drawPixmap(rcTarget,m_worm,rcSrc);
        }
        else
        {
            //游戏没结束，显示为旗子
            QRect rcSrc(0,0,m_carrot.width(),m_carrot.height());
            QRect rcTarget(frame_x+ pItem->m_pos.x()*item_width + 2,frame_y + pItem->m_pos.y()*item_height + 2,item_width-4,item_height-4);
            painter.drawPixmap(rcTarget,m_carrot,rcSrc);
        }
        painter.setBrush(Qt::transparent);
        painter.drawRect( frame_x+ pItem->m_pos.x()*item_width,frame_y + pItem->m_pos.y()*item_height,item_width,item_height);
        return;
    }
    else if (pItem->m_bOpen)
    {
        if(pItem->m_nNumber == 0)
        {
            painter.setBrush(Qt::transparent);
        }
        else
        {
            QFont font;
            font.setPointSize(20);
            font.setFamily(("msyh"));
            font.setBold(true);

            //float f = pItem->m_nNumber/8;
            //painter.setBrush(QColor(f*255,f*255,0));
            painter.setBrush(Qt::transparent);//数字背景颜色
            painter.drawRect( frame_x+ pItem->m_pos.x()*item_width,frame_y + pItem->m_pos.y()*item_height,item_width,item_height);

            painter.setBrush(Qt::black);
            painter.setFont(font);
            painter.drawText( frame_x+ pItem->m_pos.x()*item_width  + 8,frame_y + pItem->m_pos.y()*item_height + 22,QString("%1").arg(pItem->m_nNumber));
            return;
        }
    }
    else
    {
        painter.setBrush(QColor(255,170,255));
    }
    painter.drawRect( frame_x+ pItem->m_pos.x()*item_width,frame_y + pItem->m_pos.y()*item_height,item_width,item_height);
}

void MainWindow::opentheblank(QPoint pt)//点一个空白的小格子，一下子出来一大片
{
    //对于空白的小格子，如果有上下左右4个方向挨着的空白小格子，就打开并继续查找空白的小格子
    QVector<QPoint> directions;
    directions.push_back(QPoint(-1,0));
    directions.push_back(QPoint(1,0));
    directions.push_back(QPoint(0,-1));
    directions.push_back(QPoint(0,1));
    directions.push_back(QPoint(-1,-1));
    directions.push_back(QPoint(1,1));
    directions.push_back(QPoint(1,-1));
    directions.push_back(QPoint(-1,1));
    for (int i=0; i<directions.size(); i++)
    {
        QPoint ptNew = pt + directions[i];
        if (!ifinarea(ptNew))
        {
            continue;
        }
        Item* pItem = m_items[ptNew.x()][ptNew.y()];
        if (!pItem->m_bmine && !pItem->m_bOpen && !pItem->m_bMarked && pItem->m_nNumber == 0)
        {
            pItem->m_bOpen = true;

            //对于找到的空白的小格子，打开它8个方向上的带数字的小格子
            QVector<QPoint> directions2 = directions;
            directions2.push_back(QPoint(-1,-1));
            directions2.push_back(QPoint(1,1));
            directions2.push_back(QPoint(1,-1));
            directions2.push_back(QPoint(-1,1));
            directions2.push_back(QPoint(-1,0));
            directions2.push_back(QPoint(1,0));
            directions2.push_back(QPoint(0,-1));
            directions2.push_back(QPoint(0,1));
            for (int j=0; j<directions2.size(); j++)
            {
                QPoint ptNew2 = ptNew + directions2[j];
                if(!ifinarea(ptNew2))
                {
                    continue;
                }
                Item* pItem2 = m_items[ptNew2.x()][ptNew2.y()];
                if (!pItem2->m_bmine && !pItem2->m_bOpen && !pItem2->m_bMarked && pItem2->m_nNumber > 0)
                {
                    pItem2->m_bOpen = true;
                }
            }
            //递归查找上下左右4个方向的空白小格子
            opentheblank(ptNew);
        }
    }
}

bool MainWindow::ifinarea(QPoint pt)//判断鼠标点击坐标是否超过游戏区域
{
    if(pt.x()>=0 && pt.x()< m_columes && pt.y()>=0 && pt.y()< m_rows)
    {
        return true;
    }
          return false;
}
void MainWindow::on_sectime()//计时器
{
    QPoint pt;
    Item* pItem = m_items[pt.x()][pt.y()];
    pItem->timer++;
    update();
}
