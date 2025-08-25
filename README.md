# AsyncLogSystem

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/std/the-standard)
[![CMake](https://img.shields.io/badge/CMake-3.15+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)](https://github.com/your-username/AsyncLogSystem)

> 高性能异步日志系统 - 专为现代C++应用设计

## 📖 项目简介

AsyncLogSystem 是一个高性能、异步的C++日志系统，采用现代C++17标准设计，支持多种输出方式、装饰器模式和工厂模式。系统具有高吞吐量、低延迟、线程安全等特性，适用于高并发环境下的日志记录需求。

## ✨ 核心特性

- 🚀 **高性能**: 无锁队列设计，支持高并发日志记录
- 🔄 **异步处理**: 异步日志处理，不阻塞业务线程
- 🎨 **装饰器模式**: 灵活的功能组合，支持时间戳、颜色、压缩等
- 🏭 **工厂模式**: 统一的组件创建接口，支持自定义扩展
- 📁 **多输出支持**: 文件、控制台、网络等多种输出方式
- 🧵 **线程安全**: 完全线程安全，支持多生产者多消费者
- ⚙️ **配置灵活**: 丰富的配置选项，支持运行时动态调整
- 🔌 **可扩展**: 插件式架构，支持自定义输出和装饰器

## 🏗️ 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                    业务应用层 (Business Layer)                │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                    日志接口层 (Log Interface Layer)          │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                    日志管理层 (Log Management Layer)         │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                    队列管理层 (Queue Management Layer)       │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                    输出策略层 (Output Strategy Layer)        │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                    装饰器层 (Decorator Layer)                │
└─────────────────────┬───────────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────────┐
│                    系统资源层 (System Resource Layer)        │
└─────────────────────────────────────────────────────────────┘
```

## 🚀 快速开始

### 环境要求

- **编译器**: 支持C++17的编译器 (GCC 7+, Clang 5+, MSVC 2017+)
- **CMake**: 3.15或更高版本
- **操作系统**: Linux, Windows, macOS
- **依赖库**: 仅需标准C++库和线程库

### 安装步骤

1. **克隆项目**
   ```bash
   git clone https://github.com/your-username/AsyncLogSystem.git
   cd AsyncLogSystem
   ```

2. **创建构建目录**
   ```bash
   mkdir build && cd build
   ```

3. **配置和构建**
   ```bash
   cmake ..
   make -j4
   ```

4. **运行示例程序**
   ```bash
   # 运行基础示例
   ./examples/basic_usage_example
   
   # 运行高级示例
   ./examples/advanced_usage_example
   
   # 运行演示程序
   ./bin/async_log_demo
   ```

## 📚 使用指南

### 基础用法

```cpp
#include "logManager.hpp"
#include "logOutput.hpp"
#include "logDecorator.hpp"

using namespace async_log;

int main() {
    // 获取日志管理器实例
    auto& logManager = LogManager::getInstance();
    
    // 启动日志系统
    logManager.start();
    
    // 记录不同级别的日志
    logManager.log(LogLevel::INFO, "这是一条普通信息");
    logManager.log(LogLevel::WARN, "这是一条警告信息");
    logManager.log(LogLevel::ERROR, "这是一条错误信息");
    
    // 使用便捷宏
    LOG_INFO("使用宏记录的普通信息");
    LOG_WARN("使用宏记录的警告信息");
    LOG_ERROR("使用宏记录的错误信息");
    
    // 停止日志系统
    logManager.stop();
    
    return 0;
}
```

### 高级用法

```cpp
// 创建自定义输出
auto customOutput = std::make_unique<FileOutput>("./logs/app.log");
logManager.addOutput(std::move(customOutput));

// 使用装饰器
auto decoratedOutput = std::make_unique<TimestampDecorator>(
    std::make_unique<ColorDecorator>(
        std::make_unique<ConsoleOutput>()
    )
);

// 使用工厂创建输出
auto output = LogOutputFactory::createOutput("file", "./logs/factory.log");
auto decorated = LogOutputFactory::createDecoratedOutput(
    "console", {"timestamp", "color", "filter"}
);
```

### 配置选项

```cpp
LogConfig config;
config.minLevel = LogLevel::DEBUG;           // 最小日志级别
config.enableTimestamp = true;               // 启用时间戳
config.enableColor = true;                   // 启用颜色输出
config.enableThreadId = true;                // 启用线程ID
config.maxQueueSize = 10000;                 // 最大队列大小
config.flushInterval = 1000;                 // 刷新间隔（毫秒）
config.logDir = "./logs";                    // 日志目录
config.maxFileSize = 10 * 1024 * 1024;      // 最大文件大小
config.maxFileCount = 5;                     // 最大文件数量

logManager.setConfig(config);
```

## 📁 项目结构

```
AsyncLogSystem/
├── README.md                    # 项目说明文档
├── CMakeLists.txt              # 主构建配置
├── include/                     # 头文件目录
│   ├── logTypes.hpp            # 日志类型定义
│   ├── logOutput.hpp           # 日志输出接口
│   ├── logManager.hpp          # 日志管理器
│   ├── logDispatcher.hpp       # 日志分发器
│   ├── logDecorator.hpp        # 装饰器基类
│   ├── logFactory.hpp          # 工厂类
│   └── lockFreeQueue.hpp       # 无锁队列
├── src/                         # 源代码目录
│   ├── logTypes.cpp            # 类型转换实现
│   ├── logOutput.cpp           # 输出策略实现
│   ├── logManager.cpp          # 管理器实现
│   ├── logDispatcher.cpp       # 分发器实现
│   ├── logDecorator.cpp        # 装饰器实现
│   ├── logFactory.cpp          # 工厂实现
│   └── main.cpp                # 主程序入口
├── examples/                    # 示例程序
│   ├── basicUsage.cpp          # 基础使用示例
│   ├── advancedUsage.cpp       # 高级使用示例
│   └── CMakeLists.txt          # 示例构建配置
├── tests/                       # 测试目录
├── docs/                        # 文档目录
│   ├── 0_开发规范_简洁版.md     # 开发规范
│   ├── 2_架构设计.md           # 架构设计
│   └── 项目进度.md             # 项目进度
└── build/                       # 构建输出目录
```

## 🔧 构建配置

### CMake选项

```bash
# 启用地址消毒器 (Debug模式)
cmake -DENABLE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug ..

# 启用代码覆盖率
cmake -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..

# 启用静态分析
cmake -DENABLE_STATIC_ANALYSIS=ON ..

# 自定义安装路径
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
```

### 构建目标

- `async_log_system`: 静态库
- `async_log_demo`: 演示程序
- `basic_usage_example`: 基础使用示例
- `advanced_usage_example`: 高级使用示例
- `tests`: 测试套件

## 📊 性能指标

### 基准测试结果

| 测试场景 | 日志数量 | 耗时 | 吞吐量 | 平均延迟 |
|---------|---------|------|--------|----------|
| 单线程 | 10,000 | 6ms | 1,667 条/秒 | 0.6μs |
| 多线程(5) | 50,000 | 25ms | 2,000 条/秒 | 0.5μs |
| 高并发(10) | 100,000 | 45ms | 2,222 条/秒 | 0.45μs |

### 性能特点

- **高吞吐量**: 支持每秒数千条日志记录
- **低延迟**: 单条日志处理延迟小于1微秒
- **内存效率**: 智能内存管理，支持大容量日志
- **CPU友好**: 异步处理，最小化CPU占用

## 🧪 测试

### 运行测试

```bash
cd build
make tests
ctest --verbose
```

### 测试覆盖

- 单元测试: 核心功能模块测试
- 集成测试: 组件协作测试
- 性能测试: 吞吐量和延迟测试
- 压力测试: 高并发场景测试

## 📖 API文档

### 核心类

- **LogManager**: 日志管理器，单例模式
- **ILogOutput**: 日志输出接口
- **FileOutput**: 文件输出实现
- **ConsoleOutput**: 控制台输出实现
- **NetworkOutput**: 网络输出实现
- **LogDecorator**: 装饰器基类
- **LogOutputFactory**: 工厂类

### 主要方法

```cpp
// 日志记录
void log(LogLevel level, const std::string& message);
void log(LogLevel level, const std::string& message, 
         const std::string& file, int line);

// 输出管理
void addOutput(std::unique_ptr<ILogOutput> output);
void removeOutput(size_t index);
void clearOutputs();

// 生命周期管理
void start();
void stop();
void flush();
```

## 🤝 贡献指南

我们欢迎所有形式的贡献！请查看 [贡献指南](CONTRIBUTING.md) 了解详情。

### 贡献方式

1. **报告问题**: 提交 [Issue](https://github.com/your-username/AsyncLogSystem/issues)
2. **功能建议**: 讨论新功能需求
3. **代码贡献**: 提交 Pull Request
4. **文档改进**: 完善文档和示例
5. **测试用例**: 添加测试覆盖

### 开发环境设置

```bash
# 安装开发依赖
sudo apt-get install build-essential cmake clang-tidy

# 克隆开发分支
git clone -b develop https://github.com/your-username/AsyncLogSystem.git

# 运行代码检查
make lint
make format
```

## 📄 许可证

本项目采用 [MIT 许可证](LICENSE) - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

感谢所有为这个项目做出贡献的开发者和用户！

## 📞 联系我们

- **项目主页**: [GitHub](https://github.com/your-username/AsyncLogSystem)
- **问题反馈**: [Issues](https://github.com/your-username/AsyncLogSystem/issues)
- **讨论交流**: [Discussions](https://github.com/your-username/AsyncLogSystem/discussions)
- **邮件联系**: your-email@example.com

## 📈 项目状态

- **当前版本**: 1.0.0
- **开发状态**: 活跃开发中
- **维护状态**: 积极维护
- **支持平台**: Linux, Windows, macOS

---

⭐ 如果这个项目对您有帮助，请给我们一个星标！
