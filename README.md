# 异步日志系统 (Async Log System)

## 项目简介

这是一个高性能的异步日志系统，采用现代C++设计模式构建，具有高并发、低延迟、易扩展的特点。

## 核心特性

- **异步写入**: 采用生产者-消费者模式，业务线程与日志写入线程完全解耦
- **高性能**: 单例模式保证全局唯一入口，线程安全队列确保高并发性能
- **易扩展**: 策略模式抽象日志接口，支持文件、控制台等多种输出目标
- **灵活分发**: 观察者模式支持一条日志同时输出到多个目标
- **内容增强**: 装饰器模式支持时间戳、颜色、高亮、压缩等处理
- **配置驱动**: 工厂模式根据配置动态创建不同的日志实现

## 技术架构

- **设计模式**: 单例、生产者-消费者、策略、观察者、装饰器、工厂
- **并发模型**: 多线程异步处理
- **内存管理**: 智能指针、RAII
- **错误处理**: 异常安全、错误码机制

## 快速开始

```cpp
#include "log_manager.h"

// 获取日志管理器实例
auto& logger = LogManager::getInstance();

// 配置日志输出
logger.setLogLevel(LogLevel::INFO);
logger.addOutput(std::make_unique<FileOutput>("app.log"));
logger.addOutput(std::make_unique<ConsoleOutput>());

// 记录日志
logger.log(LogLevel::INFO, "Hello, Async Log System!");
```

## 项目结构

```
logSystem/
├── src/           # 源代码
├── include/       # 头文件
├── docs/          # 技术文档
├── tests/         # 单元测试
├── examples/      # 使用示例
└── build/         # 构建输出
```

## 文档

详细的技术文档请参考 `docs/` 文件夹：

- [项目设计技术方案](./docs/1_项目设计技术方案.md)
- [项目架构设计](./docs/2_项目架构设计.md)
- [开发规范](./docs/3_开发规范.md)
- [API接口](./docs/4_API接口.md)

## 构建要求

- C++17 或更高版本
- CMake 3.15+
- 支持的操作系统: Linux, Windows, macOS

## 许可证

MIT License
