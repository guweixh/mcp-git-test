import React, { useState, useEffect } from 'react';

function App() {
  const [messages, setMessages] = useState([]);
  const [inputText, setInputText] = useState('');
  
  // 用于接收来自C++后端的消息
  useEffect(() => {
    // 定义全局回调函数，供C++调用
    window.receiveMessageFromCpp = (message) => {
      setMessages(prev => [...prev, `从C++收到: ${message}`]);
    };
    
    // 通知C++，前端已准备好
    if (window.cefQuery) {
      window.cefQuery({
        request: JSON.stringify({ 
          type: 'ready',
          data: 'React前端已加载完成'
        }),
        onSuccess: (response) => {
          console.log('前端准备就绪消息已发送', response);
        },
        onFailure: (error_code, error_message) => {
          console.error('消息发送失败', error_code, error_message);
        }
      });
    }
    
    return () => {
      // 清理全局回调
      delete window.receiveMessageFromCpp;
    };
  }, []);
  
  // 向C++发送消息
  const sendMessageToCpp = () => {
    if (!inputText.trim()) return;
    
    if (window.cefQuery) {
      window.cefQuery({
        request: JSON.stringify({ 
          type: 'message',
          data: inputText
        }),
        onSuccess: (response) => {
          setMessages(prev => [...prev, `发送到C++: ${inputText}`]);
          setInputText('');
        },
        onFailure: (error_code, error_message) => {
          console.error('消息发送失败', error_code, error_message);
        }
      });
    } else {
      setMessages(prev => [...prev, `发送到C++(模拟): ${inputText}`]);
      setInputText('');
      
      // 在浏览器环境中进行测试时，模拟C++的响应
      setTimeout(() => {
        if (window.receiveMessageFromCpp) {
          window.receiveMessageFromCpp(`回复: ${inputText}`);
        }
      }, 500);
    }
  };
  
  // 请求C++执行计算操作（演示）
  const requestCalculation = () => {
    if (window.cefQuery) {
      window.cefQuery({
        request: JSON.stringify({ 
          type: 'calculate',
          data: { x: 10, y: 20 }
        }),
        onSuccess: (response) => {
          try {
            const result = JSON.parse(response);
            setMessages(prev => [...prev, `计算结果: ${result.data}`]);
          } catch (e) {
            console.error('解析响应失败', e);
          }
        },
        onFailure: (error_code, error_message) => {
          console.error('请求计算失败', error_code, error_message);
        }
      });
    }
  };
  
  // 获取系统信息示例
  const getSystemInfo = () => {
    if (window.cefQuery) {
      window.cefQuery({
        request: JSON.stringify({ 
          type: 'getSystemInfo'
        }),
        onSuccess: (response) => {
          try {
            const result = JSON.parse(response);
            setMessages(prev => [...prev, `系统信息: ${JSON.stringify(result.data)}`]);
          } catch (e) {
            console.error('解析响应失败', e);
          }
        },
        onFailure: (error_code, error_message) => {
          console.error('获取系统信息失败', error_code, error_message);
        }
      });
    }
  };

  return (
    <div className="container">
      <h1>QCefView与React通信演示</h1>
      
      <div className="message-box">
        {messages.length > 0 ? (
          <ul style={{ textAlign: 'left', listStyleType: 'none', padding: 0 }}>
            {messages.map((msg, index) => (
              <li key={index}>{msg}</li>
            ))}
          </ul>
        ) : (
          <p>暂无消息</p>
        )}
      </div>
      
      <div>
        <input
          type="text"
          value={inputText}
          onChange={(e) => setInputText(e.target.value)}
          placeholder="输入要发送给C++的消息"
          onKeyPress={(e) => e.key === 'Enter' && sendMessageToCpp()}
        />
        <button className="btn" onClick={sendMessageToCpp}>发送</button>
      </div>
      
      <div style={{ marginTop: '20px' }}>
        <button className="btn" onClick={requestCalculation}>请求计算</button>
        <button className="btn" onClick={getSystemInfo}>获取系统信息</button>
      </div>
    </div>
  );
}

export default App;