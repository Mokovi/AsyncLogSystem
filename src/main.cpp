#include "log_system.h"
#include <iostream>

int main() {
    std::cout << "=== 异步日志系统 - 阶段1 (MVP) 演示 ===" << std::endl;
    
    // 获取日志系统实例
    auto& logger = log_system::LogSystem::getInstance();
    
    // 初始化系统
    logger.initialize();
    
    // 演示不同级别的日志记录
    logger.trace("这是一条TRACE级别的日志");
    logger.debug("这是一条DEBUG级别的日志");
    logger.info("这是一条INFO级别的日志");
    logger.warn("这是一条WARN级别的日志");
    logger.error("这是一条ERROR级别的日志");
    logger.fatal("这是一条FATAL级别的日志");
    
    // 演示配置功能
    std::cout << "\n--- 修改配置后 ---" << std::endl;
    
    log_system::LogConfig config;
    config.minLevel = log_system::LogLevel::WARN;  // 只显示WARN及以上级别
    config.timeFormat = "%H:%M:%S";  // 简化时间格式
    logger.setConfig(config);
    
    // 再次记录日志，观察过滤效果
    logger.trace("TRACE日志 - 应该被过滤");
    logger.debug("DEBUG日志 - 应该被过滤");
    logger.info("INFO日志 - 应该被过滤");
    logger.warn("WARN日志 - 应该显示");
    logger.error("ERROR日志 - 应该显示");
    logger.fatal("FATAL日志 - 应该显示");
    
    // 演示禁用控制台输出
    std::cout << "\n--- 禁用控制台输出 ---" << std::endl;
    config.enableConsole = false;
    logger.setConfig(config);
    
    logger.info("这条日志不应该显示");
    logger.warn("这条日志也不应该显示");
    
    // 重新启用控制台输出
    config.enableConsole = true;
    logger.setConfig(config);
    
    std::cout << "\n--- 重新启用控制台输出 ---" << std::endl;
    logger.info("系统演示完成");
    
    // 关闭系统
    logger.shutdown();
    
    std::cout << "=== 演示结束 ===" << std::endl;
    return 0;
}
