#pragma once
#include <string>
#include <chrono>

namespace log_system {

// 日志级别枚举
enum class LogLevel {
    TRACE = 0,
    DEBUG_LEVEL = 1,  // 避免与DEBUG宏冲突
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

// 日志消息结构
struct LogMessage {
    LogLevel level;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    
    LogMessage(LogLevel l, const std::string& msg);
    std::string toString() const;
};

// 基础配置结构
struct LogConfig {
    LogLevel minLevel = LogLevel::INFO;
    bool enableConsole = true;
    std::string timeFormat = "%Y-%m-%d %H:%M:%S";
};

} // namespace log_system
