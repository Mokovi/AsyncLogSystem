#include "async_log_system.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

void demonstrateAsyncLogging() {
    std::cout << "=== 异步日志系统演示 ===" << std::endl;
    
    auto& logger = log_system::AsyncLogSystem::getInstance();
    
    // 启动异步系统
    logger.start();
    
    std::cout << "异步日志系统已启动，开始记录日志..." << std::endl;
    
    // 模拟多线程并发记录日志
    std::vector<std::thread> threads;
    const int threadCount = 5;
    const int logsPerThread = 100;
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&logger, i, logsPerThread]() {
            for (int j = 0; j < logsPerThread; ++j) {
                logger.info("线程 " + std::to_string(i) + " 记录日志 " + std::to_string(j));
                
                // 模拟一些工作负载
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "所有线程已完成，等待日志处理完成..." << std::endl;
    
    // 等待所有日志处理完成
    if (logger.waitForCompletion(5000)) {
        std::cout << "所有日志已处理完成" << std::endl;
    } else {
        std::cout << "等待超时，可能还有日志未处理完成" << std::endl;
    }
    
    // 停止异步系统
    logger.stop();
    
    std::cout << "异步日志系统演示完成" << std::endl;
}

void demonstratePerformanceComparison() {
    std::cout << "\n=== 性能对比演示 ===" << std::endl;
    
    // 测试同步日志性能
    auto& syncLogger = log_system::LogSystem::getInstance();
    syncLogger.initialize();
    
    const int logCount = 10000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < logCount; ++i) {
        syncLogger.info("同步日志 " + std::to_string(i));
    }
    auto syncEnd = std::chrono::high_resolution_clock::now();
    
    syncLogger.shutdown();
    
    // 测试异步日志性能
    auto& asyncLogger = log_system::AsyncLogSystem::getInstance();
    asyncLogger.start();
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < logCount; ++i) {
        asyncLogger.logAsync(log_system::LogLevel::INFO, "异步日志 " + std::to_string(i));
    }
    auto asyncEnd = std::chrono::high_resolution_clock::now();
    
    // 等待异步日志处理完成
    asyncLogger.waitForCompletion(10000);
    asyncLogger.stop();
    
    // 计算性能差异
    auto syncDuration = std::chrono::duration_cast<std::chrono::milliseconds>(syncEnd - start);
    auto asyncDuration = std::chrono::duration_cast<std::chrono::milliseconds>(asyncEnd - start);
    
    std::cout << "同步日志记录 " << logCount << " 条耗时: " << syncDuration.count() << "ms" << std::endl;
    std::cout << "异步日志记录 " << logCount << " 条耗时: " << asyncDuration.count() << "ms" << std::endl;
    std::cout << "性能提升: " << (double)syncDuration.count() / asyncDuration.count() << "x" << std::endl;
}

int main() {
    try {
        demonstrateAsyncLogging();
        demonstratePerformanceComparison();
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
