#include "log_types.h"
#include <sstream>
#include <iomanip>

namespace log_system {

LogMessage::LogMessage(LogLevel l, const std::string& msg) 
    : level(l), message(msg), timestamp(std::chrono::system_clock::now()) {
}

std::string LogMessage::toString() const {
    std::ostringstream oss;
    
    // 获取时间戳
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    
    // 获取级别字符串
    std::string levelStr;
    switch (level) {
        case LogLevel::TRACE: levelStr = "TRACE"; break;
        case LogLevel::DEBUG_LEVEL: levelStr = "DEBUG"; break;
        case LogLevel::INFO:  levelStr = "INFO "; break;
        case LogLevel::WARN:  levelStr = "WARN "; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
        case LogLevel::FATAL: levelStr = "FATAL"; break;
        default: levelStr = "UNKNOWN"; break;
    }
    
    oss << "[" << ss.str() << "] [" << levelStr << "] " << message;
    return oss.str();
}

} // namespace log_system
