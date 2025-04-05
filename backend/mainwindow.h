#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QCefView.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 处理CEF的回调函数
    void onInvokeMethod(const QString& name, const QVariantList& args);
    
    // 处理来自网页的请求
    void onProcessRequest(const QString& request, QCefView::CefQueryCallback callback);

private:
    Ui::MainWindow *ui;
    QCefView *m_cefView;
    
    // 向JavaScript发送消息
    void sendMessageToJs(const QString& message);
    
    // 计算示例
    int calculate(int x, int y);
    
    // 获取系统信息示例
    QJsonObject getSystemInfo();
    
    // 处理不同类型的请求
    void handleReadyMessage(const QJsonObject& data, QCefView::CefQueryCallback callback);
    void handleTextMessage(const QJsonObject& data, QCefView::CefQueryCallback callback);
    void handleCalculateRequest(const QJsonObject& data, QCefView::CefQueryCallback callback);
    void handleSystemInfoRequest(QCefView::CefQueryCallback callback);
};
#endif // MAINWINDOW_H