/**
 * @file basicUsage.cpp
 * @brief AsyncLogSystem 基础使用示例
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 演示AsyncLogSystem的基本功能和使用方法
 * @see LogManager, ILogOutput, LogDecorator
 * @since 1.0.0
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>

// 包含日志系统头文件
#include "logManager.hpp"
#include "logOutput.hpp"
#include "logDecorator.hpp"
#include "logFactory.hpp"

using namespace async_log;

/**
 * @brief 基础日志记录示例
 * @details 演示基本的日志记录功能
 */
void basicLoggingExample() {
    std::cout << "\n=== 基础日志记录示例 ===" << std::endl;
    
    // 获取日志管理器实例
    auto& logManager = LogManager::getInstance();
    
    // 记录不同级别的日志
    logManager.log(LogLevel::DEBUG, "这是一条调试信息");
    logManager.log(LogLevel::INFO, "这是一条普通信息");
    logManager.log(LogLevel::WARN, "这是一条警告信息");
    logManager.log(LogLevel::ERROR, "这是一条错误信息");
    logManager.log(LogLevel::FATAL, "这是一条致命错误信息");
    
    // 使用宏记录带位置信息的日志
    LOG_DEBUG("使用宏记录的调试信息");
    LOG_INFO("使用宏记录的普通信息");
    LOG_WARN("使用宏记录的警告信息");
    LOG_ERROR("使用宏记录的错误信息");
    LOG_FATAL("使用宏记录的致命错误信息");
    
    std::cout << "基础日志记录完成" << std::endl;
}

/**
 * @brief 多线程日志记录示例
 * @details 演示在多线程环境下的日志记录
 */
void multiThreadLoggingExample() {
    std::cout << "\n=== 多线程日志记录示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    std::vector<std::thread> threads;
    
    // 创建多个工作线程
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i, &logManager]() {
            for (int j = 0; j < 10; ++j) {
                logManager.log(LogLevel::INFO, 
                    "线程 " + std::to_string(i) + " 记录第 " + std::to_string(j) + " 条日志");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "多线程日志记录完成，共 50 条日志" << std::endl;
}

/**
 * @brief 装饰器使用示例
 * @details 演示如何使用装饰器增强日志功能
 */
void decoratorExample() {
    std::cout << "\n=== 装饰器使用示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    // 创建带时间戳的日志
    logManager.log(LogLevel::INFO, "使用时间戳装饰器的日志消息");
    
    // 创建带颜色的日志（控制台输出）
    logManager.log(LogLevel::WARN, "这是一条带颜色的警告信息");
    logManager.log(LogLevel::ERROR, "这是一条带颜色的错误信息");
    
    // 创建带过滤的日志
    logManager.log(LogLevel::DEBUG, "这条调试信息可能被过滤");
    logManager.log(LogLevel::INFO, "这条普通信息不会被过滤");
    
    std::cout << "装饰器示例完成" << std::endl;
}

/**
 * @brief 工厂模式示例
 * @details 演示如何使用工厂创建日志输出对象
 */
void factoryExample() {
    std::cout << "\n=== 工厂模式示例 ===" << std::endl;
    
    // 使用工厂创建控制台输出
    LogConfig config;
    auto consoleOutput = LogOutputFactory::createOutput("console", config);
    if (consoleOutput) {
        consoleOutput->write(LogMessage(LogLevel::INFO, "工厂创建的控制台输出测试"));
        std::cout << "成功创建控制台输出" << std::endl;
    }
    
    // 使用工厂创建文件输出
    config.logFile = "./logs/factory_test.log";
    auto fileOutput = LogOutputFactory::createOutput("file", config);
    if (fileOutput) {
        fileOutput->write(LogMessage(LogLevel::INFO, "工厂创建的文件输出测试"));
        std::cout << "成功创建文件输出" << std::endl;
    }
    
    // 使用工厂创建装饰器
    auto decoratedOutput = LogOutputFactory::createDecoratedOutput(
        "console", 
        {"timestamp", "color"},
        config
    );
    if (decoratedOutput) {
        decoratedOutput->write(LogMessage(LogLevel::INFO, "工厂创建的装饰器测试"));
        std::cout << "成功创建装饰器输出" << std::endl;
    }
    
    std::cout << "工厂模式示例完成" << std::endl;
}

/**
 * @brief 性能测试示例
 * @details 演示日志系统的性能表现
 */
void performanceTestExample() {
    std::cout << "\n=== 性能测试示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    const int logCount = 10000;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 批量记录日志
    for (int i = 0; i < logCount; ++i) {
        logManager.log(LogLevel::INFO, "性能测试日志 " + std::to_string(i));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "记录了 " << logCount << " 条日志，耗时 " << duration.count() << " 毫秒" << std::endl;
    std::cout << "平均每条日志耗时 " << (duration.count() * 1000.0 / logCount) << " 微秒" << std::endl;
}

/**
 * @brief 配置管理示例
 * @details 演示如何配置日志系统
 */
void configurationExample() {
    std::cout << "\n=== 配置管理示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    // 创建自定义配置
    LogConfig config;
    config.minLevel = LogLevel::DEBUG;
    config.enableTimestamp = true;
    config.enableColor = true;
    config.enableThreadId = true;
    config.maxQueueSize = 50000;
    config.flushInterval = 500;
    
    // 应用配置
    logManager.setConfig(config);
    
    // 添加自定义输出
    auto customOutput = std::make_unique<FileOutput>("./logs/custom.log");
    logManager.addOutput(std::move(customOutput));
    
    // 测试配置
    logManager.log(LogLevel::DEBUG, "配置后的调试信息");
    logManager.log(LogLevel::INFO, "配置后的普通信息");
    
    std::cout << "配置管理示例完成" << std::endl;
}

/**
 * @brief 错误处理示例
 * @details 演示日志系统的错误处理能力
 */
void errorHandlingExample() {
    std::cout << "\n=== 错误处理示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    try {
        // 尝试创建无效的文件输出
        auto invalidOutput = std::make_unique<FileOutput>("/invalid/path/test.log");
        logManager.addOutput(std::move(invalidOutput));
        
        logManager.log(LogLevel::ERROR, "这条日志应该被记录到控制台");
        
    } catch (const std::exception& e) {
        std::cout << "捕获到异常: " << e.what() << std::endl;
        logManager.log(LogLevel::ERROR, "异常处理测试: " + std::string(e.what()));
    }
    
    // 测试队列满的情况
    for (int i = 0; i < 1000; ++i) {
        logManager.log(LogLevel::INFO, "压力测试日志 " + std::to_string(i));
    }
    
    std::cout << "错误处理示例完成" << std::endl;
}

/**
 * @brief 主函数
 * @return 程序退出码
 */
int main() {
    std::cout << "AsyncLogSystem 基础使用示例程序" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // 启动日志系统
        auto& logManager = LogManager::getInstance();
        logManager.start();
        
        std::cout << "日志系统启动成功\n" << std::endl;
        
        // 运行各种示例
        basicLoggingExample();
        multiThreadLoggingExample();
        decoratorExample();
        factoryExample();
        performanceTestExample();
        configurationExample();
        errorHandlingExample();
        
        // 等待日志处理完成
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // 停止日志系统
        logManager.stop();
        
        std::cout << "\n所有示例执行完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "程序执行出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
