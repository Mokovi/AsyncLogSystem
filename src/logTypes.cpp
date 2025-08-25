/**
 * @file logTypes.cpp
 * @brief 日志类型相关函数实现
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现日志级别转换等辅助函数
 * @see log_types.hpp
 * @since 1.0.0
 */

#include "logTypes.hpp"
#include <sstream>
#include <iomanip>
#include <thread>

namespace async_log {

std::string levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default:              return "UNKNOWN";
    }
}

LogLevel stringToLevel(const std::string& levelStr) {
    if (levelStr == "DEBUG" || levelStr == "debug") return LogLevel::DEBUG;
    if (levelStr == "INFO" || levelStr == "info") return LogLevel::INFO;
    if (levelStr == "WARN" || levelStr == "warn") return LogLevel::WARN;
    if (levelStr == "ERROR" || levelStr == "error") return LogLevel::ERROR;
    if (levelStr == "FATAL" || levelStr == "fatal") return LogLevel::FATAL;
    
    // 默认返回INFO级别
    return LogLevel::INFO;
}

} // namespace async_log
