#include "log_system.h"
#include <iostream>
#include <thread>
#include <chrono>

void demonstrateBasicUsage() {
    std::cout << "\n=== 基础使用示例 ===" << std::endl;
    
    auto& logger = log_system::LogSystem::getInstance();
    logger.initialize();
    
    // 基本日志记录
    logger.info("系统启动成功");
    logger.debug("调试信息：当前配置已加载");
    logger.warn("警告：磁盘空间不足");
    logger.error("错误：数据库连接失败");
    
    logger.shutdown();
}

void demonstrateConfigChanges() {
    std::cout << "\n=== 配置变更示例 ===" << std::endl;
    
    auto& logger = log_system::LogSystem::getInstance();
    logger.initialize();
    
    // 默认配置
    logger.info("使用默认配置记录日志");
    
    // 修改配置
    log_system::LogConfig config;
    config.minLevel = log_system::LogLevel::DEBUG_LEVEL;
    config.timeFormat = "%H:%M:%S";
    logger.setConfig(config);
    
    logger.debug("现在可以看到DEBUG级别的日志了");
    logger.info("时间格式也变得更简洁了");
    
    logger.shutdown();
}

void demonstrateLogLevels() {
    std::cout << "\n=== 日志级别示例 ===" << std::endl;
    
    auto& logger = log_system::LogSystem::getInstance();
    logger.initialize();
    
    // 设置只显示ERROR和FATAL级别
    log_system::LogConfig config;
    config.minLevel = log_system::LogLevel::ERROR;
    logger.setConfig(config);
    
    logger.trace("TRACE - 不会显示");
    logger.debug("DEBUG - 不会显示");
    logger.info("INFO - 不会显示");
    logger.warn("WARN - 不会显示");
    logger.error("ERROR - 会显示");
    logger.fatal("FATAL - 会显示");
    
    logger.shutdown();
}

void demonstratePerformance() {
    std::cout << "\n=== 性能测试示例 ===" << std::endl;
    
    auto& logger = log_system::LogSystem::getInstance();
    logger.initialize();
    
    const int testCount = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < testCount; ++i) {
        logger.info("性能测试日志 #" + std::to_string(i));
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "记录了 " << testCount << " 条日志，耗时: " 
              << duration.count() << " 毫秒" << std::endl;
    std::cout << "平均每条日志耗时: " 
              << (double)duration.count() / testCount << " 毫秒" << std::endl;
    
    logger.shutdown();
}

int main() {
    std::cout << "异步日志系统 - 阶段1 (MVP) 使用示例" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // 基础使用
        demonstrateBasicUsage();
        
        // 配置变更
        demonstrateConfigChanges();
        
        // 日志级别
        demonstrateLogLevels();
        
        // 性能测试
        demonstratePerformance();
        
        std::cout << "\n=== 所有示例执行完成 ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "示例执行出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
