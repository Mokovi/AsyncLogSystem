#include "log_system.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace log_system {

LogSystem& LogSystem::getInstance() {
    static LogSystem instance;
    return instance;
}

void LogSystem::log(LogLevel level, const std::string& message) {
    if (level < config.minLevel) return;
    
    LogMessage msg(level, message);
    writeToConsole(msg);
}

void LogSystem::trace(const std::string& message) { log(LogLevel::TRACE, message); }
void LogSystem::debug(const std::string& message) { log(LogLevel::DEBUG_LEVEL, message); }
void LogSystem::info(const std::string& message) { log(LogLevel::INFO, message); }
void LogSystem::warn(const std::string& message) { log(LogLevel::WARN, message); }
void LogSystem::error(const std::string& message) { log(LogLevel::ERROR, message); }
void LogSystem::fatal(const std::string& message) { log(LogLevel::FATAL, message); }

void LogSystem::writeToConsole(const LogMessage& msg) {
    if (!config.enableConsole) return;
    std::cout << formatMessage(msg) << std::endl;
}

std::string LogSystem::formatMessage(const LogMessage& msg) {
    std::ostringstream oss;
    
    // 使用配置中的时间格式
    oss << "[" << getCurrentTimestamp() << "] ";
    
    // 获取级别字符串
    std::string levelStr;
    switch (msg.level) {
        case LogLevel::TRACE: levelStr = "TRACE"; break;
        case LogLevel::DEBUG_LEVEL: levelStr = "DEBUG"; break;
        case LogLevel::INFO:  levelStr = "INFO "; break;
        case LogLevel::WARN:  levelStr = "WARN "; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        case LogLevel::FATAL: levelStr = "FATAL"; break;
        default: levelStr = "UNKNOWN"; break;
    }
    
    oss << "[" << levelStr << "] " << msg.message;
    return oss.str();
}

std::string LogSystem::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), config.timeFormat.c_str());
    return ss.str();
}

void LogSystem::setConfig(const LogConfig& newConfig) {
    config = newConfig;
}

const LogConfig& LogSystem::getConfig() const {
    return config;
}

void LogSystem::initialize() {
    initialized = true;
}

void LogSystem::shutdown() {
    initialized = false;
}

} // namespace log_system
