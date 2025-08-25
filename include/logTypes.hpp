/**
 * @file logTypes.hpp
 * @brief 日志系统基础类型定义
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 定义日志系统中使用的基础数据类型，包括日志级别、消息结构、配置选项等
 * @see ILogOutput, LogManager
 * @since 1.0.0
 */

#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <vector>
#include <thread>

namespace async_log {

/**
 * @brief 日志级别枚举
 * @details 定义了从DEBUG到FATAL的五个日志级别，用于控制日志输出的详细程度
 * @since 1.0.0
 */
enum class LogLevel : uint8_t {
    DEBUG = 0,    ///< 调试信息，最详细的日志级别
    INFO = 1,     ///< 一般信息，记录程序运行状态
    WARN = 2,     ///< 警告信息，潜在问题但不影响程序运行
    ERROR = 3,    ///< 错误信息，程序运行出现问题
    FATAL = 4     ///< 致命错误，程序无法继续运行
};

/**
 * @brief 日志消息结构体
 * @details 包含一条完整日志的所有信息，包括级别、内容、时间戳、源文件等
 * @since 1.0.0
 */
struct LogMessage {
    LogLevel level;                    ///< 日志级别
    std::string message;               ///< 日志内容
    std::string file;                  ///< 源文件名
    int line;                          ///< 源文件行号
    std::string function;              ///< 函数名
    std::chrono::system_clock::time_point timestamp; ///< 时间戳
    std::thread::id threadId;          ///< 线程ID
    
    /**
     * @brief 默认构造函数
     * @since 1.0.0
     */
    LogMessage() : level(LogLevel::INFO), line(0), 
                   timestamp(std::chrono::system_clock::now()),
                   threadId(std::this_thread::get_id()) {}
    
    /**
     * @brief 构造函数
     * @param[in] lvl 日志级别
     * @param[in] msg 日志消息
     * @param[in] f 源文件名
     * @param[in] ln 源文件行号
     * @param[in] func 函数名
     * @since 1.0.0
     */
    LogMessage(LogLevel lvl, const std::string& msg, 
               const std::string& f = "", int ln = 0, 
               const std::string& func = "")
        : level(lvl), message(msg), file(f), line(ln), 
          function(func), timestamp(std::chrono::system_clock::now()),
          threadId(std::this_thread::get_id()) {}
};

/**
 * @brief 日志配置结构体
 * @details 包含日志系统的各种配置选项，如输出目标、格式、级别等
 * @since 1.0.0
 */
struct LogConfig {
    LogLevel minLevel = LogLevel::DEBUG;    ///< 最小日志级别
    std::string format = "[{level}] {time} {file}:{line} - {message}"; ///< 日志格式
    size_t maxQueueSize = 10000;           ///< 最大队列大小
    size_t flushInterval = 1000;           ///< 刷新间隔（毫秒）
    bool enableTimestamp = true;           ///< 是否启用时间戳
    bool enableColor = true;               ///< 是否启用颜色输出
    bool enableThreadId = true;            ///< 是否启用线程ID
    std::string logDir = "./logs";         ///< 日志目录
    std::string logFile = "app.log";       ///< 日志文件名
    size_t maxFileSize = 10 * 1024 * 1024; ///< 最大文件大小（字节）
    int maxFileCount = 5;                  ///< 最大文件数量
};

/**
 * @brief 日志级别字符串转换函数
 * @param[in] level 日志级别
 * @return 对应的字符串表示
 * @since 1.0.0
 */
std::string levelToString(LogLevel level);

/**
 * @brief 字符串转日志级别函数
 * @param[in] levelStr 日志级别字符串
 * @return 对应的日志级别枚举值
 * @since 1.0.0
 */
LogLevel stringToLevel(const std::string& levelStr);

} // namespace async_log
