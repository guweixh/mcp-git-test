# QCefView与React通信演示

这是一个使用QCefView(C++/Qt)和React实现前后端通信的演示项目。项目展示了如何在现代Qt桌面应用中嵌入React前端并实现双向通信。

## 项目结构

```
.
├── frontend/             # React前端
│   ├── public/           # 静态资源
│   ├── src/              # React源代码
│   └── package.json      # 依赖配置
│
└── backend/              # C++/Qt后端
    ├── CMakeLists.txt    # CMake构建配置
    ├── main.cpp          # 主函数
    ├── mainwindow.h      # 主窗口头文件
    ├── mainwindow.cpp    # 主窗口实现
    ├── mainwindow.ui     # Qt UI文件
    └── resources.qrc     # Qt资源文件
```

## 技术栈

- **前端**: React 18
- **后端**: Qt 5 + QCefView (基于Chromium Embedded Framework)

## 前后端通信方式

本项目实现了两种通信方式：

1. **C++调用JavaScript**：通过`QCefView::executeJavaScript`方法执行JavaScript代码
2. **JavaScript调用C++**：通过CEF的`cefQuery`接口发送请求，并在C++端处理

## 构建指南

### 前端构建

```bash
# 进入前端目录
cd frontend

# 安装依赖
npm install

# 开发模式
npm start

# 构建生产版本
npm run build
```

### 后端构建

#### 前提条件

- Qt 5.12+
- CMake 3.14+
- QCefView库 (https://github.com/tishion/QCefView)

#### 构建步骤

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目，设置QCefView路径
cmake -DQCEF_ROOT_DIR=path/to/QCefView ..

# 构建
cmake --build .
```

## 通信示例

### C++调用JavaScript

```cpp
// 在C++代码中
void MainWindow::sendMessageToJs(const QString& message)
{
    QString script = QString("window.receiveMessageFromCpp('%1')").arg(message);
    m_cefView->executeJavaScript(script);
}
```

### JavaScript调用C++

```javascript
// 在React代码中
const sendMessageToCpp = () => {
  if (window.cefQuery) {
    window.cefQuery({
      request: JSON.stringify({ 
        type: 'message',
        data: inputText
      }),
      onSuccess: (response) => {
        console.log('成功:', response);
      },
      onFailure: (error_code, error_message) => {
        console.error('失败:', error_code, error_message);
      }
    });
  }
};
```

## 功能演示

- 文本消息互相发送
- C++执行计算任务并返回结果
- 获取系统信息
- 回调函数响应

## 注意事项

- 需要将QCefView的依赖库(如CEF)添加到运行环境
- 如果是在开发模式下，可以连接到React的开发服务器
- 需要确保前端构建后的文件被正确复制到后端程序所在目录