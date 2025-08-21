#pragma once
#include "log_types.h"
#include <memory>

namespace log_system {

class LogSystem {
public:
    static LogSystem& getInstance();
    
    // 基础日志记录
    void log(LogLevel level, const std::string& message);
    void trace(const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);
    
    // 配置管理
    void setConfig(const LogConfig& config);
    const LogConfig& getConfig() const;
    
    // 系统控制
    void initialize();
    void shutdown();
    
private:
    LogSystem() = default;
    ~LogSystem() = default;
    LogSystem(const LogSystem&) = delete;
    LogSystem& operator=(const LogSystem&) = delete;
    
    void writeToConsole(const LogMessage& msg);
    std::string formatMessage(const LogMessage& msg);
    std::string getCurrentTimestamp();
    
    LogConfig config;
    bool initialized = false;
};

} // namespace log_system
