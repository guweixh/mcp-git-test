#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QSysInfo>
#include <QScreen>
#include <QApplication>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 创建QCefView
    m_cefView = new QCefView(this);
    setCentralWidget(m_cefView);
    
    // 设置窗口大小和标题
    resize(800, 600);
    setWindowTitle("QCefView React Demo");
    
    // 获取前端页面URL
    // 注意：这里使用相对路径找到构建目录中的web文件夹
    QString webDir = QDir::currentPath() + "/web";
    QFileInfo indexFile(webDir + "/index.html");
    
    if (!indexFile.exists()) {
        qWarning() << "前端文件未找到，尝试开发模式路径";
        // 开发模式时的替代路径
        webDir = QDir::currentPath() + "/../frontend/build";
        indexFile.setFile(webDir + "/index.html");
    }
    
    QString urlStr;
    if (indexFile.exists()) {
        urlStr = QUrl::fromLocalFile(indexFile.absoluteFilePath()).toString();
        qDebug() << "加载URL:" << urlStr;
    } else {
        qWarning() << "无法找到前端文件，使用开发服务器";
        // 如果在开发模式，可以连接到React开发服务器
        urlStr = "http://localhost:3000";
    }
    
    // 连接QCefView的信号
    connect(m_cefView, &QCefView::invokeMethod, this, &MainWindow::onInvokeMethod);
    connect(m_cefView, &QCefView::processUrlRequest, this, &MainWindow::onProcessRequest);
    
    // 加载URL
    m_cefView->navigateToUrl(urlStr);
    
    // 示例：5秒后向前端发送一条测试消息
    QTimer::singleShot(5000, this, [this]() {
        sendMessageToJs("C++后端初始化完成，这是一条来自C++的消息");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onInvokeMethod(const QString& name, const QVariantList& args)
{
    qDebug() << "JS调用了C++方法:" << name << "参数:" << args;
    
    // 根据方法名处理不同的调用
    if (name == "nativeAlert") {
        if (!args.isEmpty()) {
            QString message = args.at(0).toString();
            qDebug() << "收到JS的消息:" << message;
            
            // 处理消息逻辑
            QString response = "C++收到消息: " + message;
            sendMessageToJs(response);
        }
    }
}

void MainWindow::onProcessRequest(const QString& request, QCefView::CefQueryCallback callback)
{
    qDebug() << "收到CEF请求:" << request;
    
    // 解析JSON请求
    QJsonDocument doc = QJsonDocument::fromJson(request.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "无效的JSON请求";
        callback.failure(1, "无效的JSON请求");
        return;
    }
    
    QJsonObject jsonObj = doc.object();
    QString type = jsonObj["type"].toString();
    
    // 根据请求类型处理
    if (type == "ready") {
        handleReadyMessage(jsonObj, callback);
    } else if (type == "message") {
        handleTextMessage(jsonObj, callback);
    } else if (type == "calculate") {
        handleCalculateRequest(jsonObj, callback);
    } else if (type == "getSystemInfo") {
        handleSystemInfoRequest(callback);
    } else {
        callback.failure(2, "未知的请求类型");
    }
}

void MainWindow::handleReadyMessage(const QJsonObject& obj, QCefView::CefQueryCallback callback)
{
    QString data = obj["data"].toString();
    qDebug() << "前端就绪:" << data;
    
    QJsonObject response;
    response["status"] = "ok";
    response["message"] = "C++后端已收到前端就绪消息";
    
    QJsonDocument doc(response);
    callback.success(doc.toJson(QJsonDocument::Compact));
}

void MainWindow::handleTextMessage(const QJsonObject& obj, QCefView::CefQueryCallback callback)
{
    QString message = obj["data"].toString();
    qDebug() << "收到前端消息:" << message;
    
    // 示例：简单处理消息
    QString response = "已收到消息: " + message;
    
    // 也可以通过JavaScript回调响应
    QTimer::singleShot(500, this, [this, message]() {
        sendMessageToJs("C++处理结果: " + message);
    });
    
    QJsonObject responseObj;
    responseObj["status"] = "ok";
    responseObj["message"] = response;
    
    QJsonDocument doc(responseObj);
    callback.success(doc.toJson(QJsonDocument::Compact));
}

void MainWindow::handleCalculateRequest(const QJsonObject& obj, QCefView::CefQueryCallback callback)
{
    QJsonObject data = obj["data"].toObject();
    int x = data["x"].toInt();
    int y = data["y"].toInt();
    
    int result = calculate(x, y);
    
    QJsonObject response;
    response["status"] = "ok";
    response["data"] = result;
    
    QJsonDocument doc(response);
    callback.success(doc.toJson(QJsonDocument::Compact));
}

void MainWindow::handleSystemInfoRequest(QCefView::CefQueryCallback callback)
{
    QJsonObject sysInfo = getSystemInfo();
    
    QJsonObject response;
    response["status"] = "ok";
    response["data"] = sysInfo;
    
    QJsonDocument doc(response);
    callback.success(doc.toJson(QJsonDocument::Compact));
}

int MainWindow::calculate(int x, int y)
{
    // 示例简单计算
    return x + y;
}

QJsonObject MainWindow::getSystemInfo()
{
    QJsonObject info;
    
    // 获取系统信息
    info["productName"] = QSysInfo::prettyProductName();
    info["kernelType"] = QSysInfo::kernelType();
    info["kernelVersion"] = QSysInfo::kernelVersion();
    info["machineHostName"] = QSysInfo::machineHostName();
    
    // 屏幕信息
    QScreen *screen = QApplication::primaryScreen();
    QJsonObject screenInfo;
    screenInfo["width"] = screen->size().width();
    screenInfo["height"] = screen->size().height();
    screenInfo["refreshRate"] = screen->refreshRate();
    info["screen"] = screenInfo;
    
    return info;
}

void MainWindow::sendMessageToJs(const QString& message)
{
    // 通过调用JavaScript方法发送消息
    QString script = QString("window.receiveMessageFromCpp('%1')").arg(message);
    m_cefView->executeJavaScript(script);
}