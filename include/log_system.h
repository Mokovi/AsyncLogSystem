#pragma once
#include "log_types.h"
#include <memory>

namespace log_system {

class LogSystem {
public:
    static LogSystem& getInstance();
    
    // 基础日志记录
    virtual void log(LogLevel level, const std::string& message);
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
    
    // 虚析构函数，允许继承
    virtual ~LogSystem() = default;
    
protected:
    // 保护构造函数，允许继承
    LogSystem() = default;
    
    // 保护方法，允许子类访问
    void writeToConsole(const LogMessage& msg);
    std::string formatMessage(const LogMessage& msg);
    std::string getCurrentTimestamp();
    
    LogConfig config;
    bool initialized = false;

private:
    // 禁用拷贝构造和赋值
    LogSystem(const LogSystem&) = delete;
    LogSystem& operator=(const LogSystem&) = delete;
};

} // namespace log_system
