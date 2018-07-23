#include "mainwindow.h"
#include <QApplication>
#include<QSplashScreen>
//#include<qDebug>
//#include<QByteArray>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //QByteArray temp;
    //temp.fromStdString(QCoreApplication::applicationDirPath().toStdString());
    //printf("1111111%s",temp.data());

    QPixmap pixmap(":/new/image/welcome.png");        //创建QPixmap对象，设置欢迎图片
    QSplashScreen splash(pixmap);                     //利用QPixmap对象创建一个QSplashScreen对象
    splash.show ();                                   //显示此欢迎图片
    a.processEvents ();                               //使程序在显示欢迎图片的同时能能响应鼠标等其他事件

    MainWindow w;
    w.show();

    splash.finish (&w);//表示在主窗体对象初始化完成后，结束启动画面

    return a.exec();
}
