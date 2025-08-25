/**
 * @file main.cpp
 * @brief 主程序入口
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 演示异步日志系统的基本功能和使用方法
 * @see LogManager, ILogOutput
 * @since 1.0.0
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <memory>

#include "logManager.hpp"
#include "logOutput.hpp"
#include "logDecorator.hpp"
#include "logFactory.hpp"

using namespace async_log;

/**
 * @brief 演示基本日志功能
 * @since 1.0.0
 */
void demonstrateBasicLogging() {
    std::cout << "\n=== 基本日志功能演示 ===" << std::endl;
    
    // 获取日志管理器实例
    auto& logManager = LogManager::getInstance();
    
    // 记录不同级别的日志
    logManager.debug("这是一条调试信息");
    logManager.info("这是一条普通信息");
    logManager.warn("这是一条警告信息");
    logManager.error("这是一条错误信息");
    logManager.fatal("这是一条致命错误信息");
    
    // 使用带位置信息的日志
    logManager.log(LogLevel::INFO, "带位置信息的日志", __FILE__, __LINE__, __FUNCTION__);
    
    // 使用便捷宏
    LOG_DEBUG("使用宏记录的调试信息");
    LOG_INFO("使用宏记录的普通信息");
    LOG_WARN("使用宏记录的警告信息");
    LOG_ERROR("使用宏记录的错误信息");
    LOG_FATAL("使用宏记录的致命错误信息");
    
    // 使用带函数名的宏
    LOG_DEBUG_FUNC("使用函数名宏记录的调试信息");
    LOG_INFO_FUNC("使用函数名宏记录的普通信息");
}

/**
 * @brief 演示多线程日志记录
 * @since 1.0.0
 */
void demonstrateMultiThreadLogging() {
    std::cout << "\n=== 多线程日志记录演示 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    // 创建多个线程同时记录日志
    std::vector<std::thread> threads;
    const int threadCount = 5;
    const int logCount = 10;
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([i, logCount, &logManager]() {
            for (int j = 0; j < logCount; ++j) {
                logManager.info("线程 " + std::to_string(i) + " 记录的第 " + 
                               std::to_string(j + 1) + " 条日志");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "多线程日志记录完成，共 " << threadCount * logCount << " 条日志" << std::endl;
}

/**
 * @brief 演示装饰器功能
 * @since 1.0.0
 */
void demonstrateDecorators() {
    std::cout << "\n=== 装饰器功能演示 ===" << std::endl;
    
    // 创建带装饰器的输出
    auto consoleOutput = std::make_unique<ConsoleOutput>(true);
    auto timestampDecorator = std::make_unique<TimestampDecorator>(std::move(consoleOutput));
    auto colorDecorator = std::make_unique<ColorDecorator>(std::move(timestampDecorator));
    
    // 使用装饰器输出日志
    LogMessage msg(LogLevel::INFO, "使用装饰器装饰的日志消息");
    colorDecorator->write(msg);
    
    std::cout << "装饰器演示完成" << std::endl;
}

/**
 * @brief 演示工厂模式
 * @since 1.0.0
 */
void demonstrateFactory() {
    std::cout << "\n=== 工厂模式演示 ===" << std::endl;
    
    LogConfig config;
    config.enableColor = true;
    config.enableTimestamp = true;
    
    // 使用工厂创建输出对象
    auto consoleOutput = LogOutputFactory::createOutput("console", config);
    if (consoleOutput) {
        std::cout << "成功创建控制台输出" << std::endl;
        
        // 使用工厂创建装饰器
        auto timestampDecorator = LogOutputFactory::createDecorator("timestamp", 
                                                                  std::move(consoleOutput), 
                                                                  config);
        if (timestampDecorator) {
            std::cout << "成功创建时间戳装饰器" << std::endl;
            
            // 测试装饰器
            LogMessage msg(LogLevel::INFO, "工厂创建的装饰器测试");
            timestampDecorator->write(msg);
        }
    }
    
    std::cout << "工厂模式演示完成" << std::endl;
}

/**
 * @brief 演示性能测试
 * @since 1.0.0
 */
void demonstratePerformance() {
    std::cout << "\n=== 性能测试演示 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    const int logCount = 10000;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 批量记录日志
    for (int i = 0; i < logCount; ++i) {
        logManager.info("性能测试日志 " + std::to_string(i + 1));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "记录了 " << logCount << " 条日志，耗时 " << duration.count() << " 毫秒" << std::endl;
    std::cout << "平均每条日志耗时 " << (duration.count() * 1000.0 / logCount) << " 微秒" << std::endl;
    
    // 等待日志处理完成
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

/**
 * @brief 主函数
 * @return 程序退出码
 * @since 1.0.0
 */
int main() {
    std::cout << "AsyncLogSystem 异步日志系统演示程序" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // 启动日志系统
        auto& logManager = LogManager::getInstance();
        if (!logManager.start()) {
            std::cerr << "启动日志系统失败" << std::endl;
            return 1;
        }
        
        std::cout << "日志系统启动成功" << std::endl;
        
        // 演示各种功能
        demonstrateBasicLogging();
        demonstrateMultiThreadLogging();
        demonstrateDecorators();
        demonstrateFactory();
        demonstratePerformance();
        
        // 停止日志系统
        logManager.stop();
        std::cout << "日志系统已停止" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "程序运行出错: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n演示程序运行完成" << std::endl;
    return 0;
}
