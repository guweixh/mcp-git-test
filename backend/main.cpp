#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    // 设置CEF命令行参数
    // 这些参数可以根据需要调整
    QCefGlobalSettings::initializeInstance();
    QCefGlobalSettings::getInstance().setCachePath("cache");
    QCefGlobalSettings::getInstance().setUserDataPath("user-data");
    QCefGlobalSettings::getInstance().setBridgeObjectName("CefViewBridge");
    QCefGlobalSettings::getInstance().setRemoteDebuggingPort(9999);  // 方便调试
    
    qDebug() << "当前工作目录:" << QDir::currentPath();
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    int result = a.exec();
    
    // 销毁CEF
    QCefGlobalSettings::destroyInstance();
    
    return result;
}