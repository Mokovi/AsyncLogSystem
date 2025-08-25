/**
 * @file logFactory.cpp
 * @brief 日志工厂类实现
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现工厂模式，根据配置创建和管理日志输出对象
 * @see log_factory.hpp
 * @since 1.0.0
 */

#include "logFactory.hpp"
#include "logOutput.hpp"
#include "logDecorator.hpp"
#include <algorithm>
#include <stdexcept>

namespace async_log {

// 静态成员初始化
std::unordered_map<std::string, LogOutputFactory::OutputCreator> LogOutputFactory::outputCreators_;
std::unordered_map<std::string, LogOutputFactory::DecoratorCreator> LogOutputFactory::decoratorCreators_;
std::mutex LogOutputFactory::factoryMutex_;

// 静态初始化函数
namespace {
    bool initializeFactory() {
        LogOutputFactory::initializeBuiltinTypes();
        return true;
    }
    
    static bool initialized = initializeFactory();
}

std::unique_ptr<ILogOutput> LogOutputFactory::createOutput(const std::string& type, 
                                                          const LogConfig& config) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    auto it = outputCreators_.find(type);
    if (it != outputCreators_.end()) {
        return it->second(config);
    }
    
    return nullptr;
}

std::unique_ptr<ILogOutput> LogOutputFactory::createOutput(OutputType type, 
                                                          const LogConfig& config) {
    std::string typeStr = outputTypeToString(type);
    return createOutput(typeStr, config);
}

std::unique_ptr<LogDecorator> LogOutputFactory::createDecorator(const std::string& type,
                                                               std::unique_ptr<ILogOutput> output,
                                                               const LogConfig& config) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    auto it = decoratorCreators_.find(type);
    if (it != decoratorCreators_.end()) {
        return it->second(std::move(output), config);
    }
    
    return nullptr;
}

std::unique_ptr<LogDecorator> LogOutputFactory::createDecorator(DecoratorType type,
                                                               std::unique_ptr<ILogOutput> output,
                                                               const LogConfig& config) {
    std::string typeStr = decoratorTypeToString(type);
    return createDecorator(typeStr, std::move(output), config);
}

bool LogOutputFactory::registerOutputType(const std::string& type, OutputCreator creator) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    if (creator) {
        outputCreators_[type] = std::move(creator);
        return true;
    }
    
    return false;
}

bool LogOutputFactory::registerDecoratorType(const std::string& type, DecoratorCreator creator) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    if (creator) {
        decoratorCreators_[type] = std::move(creator);
        return true;
    }
    
    return false;
}

bool LogOutputFactory::unregisterOutputType(const std::string& type) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    auto it = outputCreators_.find(type);
    if (it != outputCreators_.end()) {
        outputCreators_.erase(it);
        return true;
    }
    
    return false;
}

bool LogOutputFactory::unregisterDecoratorType(const std::string& type) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    auto it = decoratorCreators_.find(type);
    if (it != decoratorCreators_.end()) {
        decoratorCreators_.erase(it);
        return true;
    }
    
    return false;
}

bool LogOutputFactory::isOutputTypeRegistered(const std::string& type) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    return outputCreators_.find(type) != outputCreators_.end();
}

bool LogOutputFactory::isDecoratorTypeRegistered(const std::string& type) {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    return decoratorCreators_.find(type) != decoratorCreators_.end();
}

std::vector<std::string> LogOutputFactory::getRegisteredOutputTypes() {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    std::vector<std::string> types;
    types.reserve(outputCreators_.size());
    
    for (const auto& [type, _] : outputCreators_) {
        types.push_back(type);
    }
    
    return types;
}

std::vector<std::string> LogOutputFactory::getRegisteredDecoratorTypes() {
    std::lock_guard<std::mutex> lock(factoryMutex_);
    
    std::vector<std::string> types;
    types.reserve(decoratorCreators_.size());
    
    for (const auto& [type, _] : decoratorCreators_) {
        types.push_back(type);
    }
    
    return types;
}

std::unique_ptr<ILogOutput> LogOutputFactory::createDecoratedOutput(
    const std::string& outputType,
    const std::vector<std::string>& decoratorTypes,
    const LogConfig& config) {
    
    // 创建基础输出
    auto output = createOutput(outputType, config);
    if (!output) {
        return nullptr;
    }
    
    // 应用装饰器
    for (const auto& decoratorType : decoratorTypes) {
        auto decorator = createDecorator(decoratorType, std::move(output), config);
        if (decorator) {
            output = std::move(decorator);
        }
    }
    
    return output;
}

std::unique_ptr<ILogOutput> LogOutputFactory::createFromConfig(const LogConfig& config) {
    // 默认创建控制台输出
    auto output = createOutput("console", config);
    
    // 根据配置添加装饰器
    if (config.enableTimestamp) {
        output = createDecorator("timestamp", std::move(output), config);
    }
    
    if (config.enableColor) {
        output = createDecorator("color", std::move(output), config);
    }
    
    return output;
}

// 内置输出类型创建函数
std::unique_ptr<ILogOutput> LogOutputFactory::createFileOutput(const LogConfig& config) {
    return std::make_unique<FileOutput>(config.logDir + "/" + config.logFile,
                                       config.maxFileSize,
                                       config.maxFileCount);
}

std::unique_ptr<ILogOutput> LogOutputFactory::createConsoleOutput(const LogConfig& config) {
    return std::make_unique<ConsoleOutput>(config.enableColor);
}

std::unique_ptr<ILogOutput> LogOutputFactory::createNetworkOutput(const LogConfig& config) {
    // 这里应该从配置中读取网络参数
    // 暂时使用默认值
    return std::make_unique<NetworkOutput>("localhost", 8080);
}

// 内置装饰器创建函数
std::unique_ptr<LogDecorator> LogOutputFactory::createTimestampDecorator(
    std::unique_ptr<ILogOutput> output, const LogConfig& config) {
    return std::make_unique<TimestampDecorator>(std::move(output));
}

std::unique_ptr<LogDecorator> LogOutputFactory::createColorDecorator(
    std::unique_ptr<ILogOutput> output, const LogConfig& config) {
    return std::make_unique<ColorDecorator>(std::move(output), config.enableColor);
}

std::unique_ptr<LogDecorator> LogOutputFactory::createCompressionDecorator(
    std::unique_ptr<ILogOutput> output, const LogConfig& config) {
    return std::make_unique<CompressionDecorator>(std::move(output));
}

std::unique_ptr<LogDecorator> LogOutputFactory::createFilterDecorator(
    std::unique_ptr<ILogOutput> output, const LogConfig& config) {
    // 创建默认过滤器：只允许指定级别以上的日志
    auto filter = [config](const LogMessage& msg) {
        return static_cast<int>(msg.level) >= static_cast<int>(config.minLevel);
    };
    
    return std::make_unique<FilterDecorator>(std::move(output), filter);
}

std::unique_ptr<LogDecorator> LogOutputFactory::createFormatDecorator(
    std::unique_ptr<ILogOutput> output, const LogConfig& config) {
    return std::make_unique<FormatDecorator>(std::move(output), config.format);
}

void LogOutputFactory::initializeBuiltinTypes() {
    // 注册内置输出类型
    outputCreators_["file"] = createFileOutput;
    outputCreators_["console"] = createConsoleOutput;
    outputCreators_["network"] = createNetworkOutput;
    
    // 注册内置装饰器类型
    decoratorCreators_["timestamp"] = createTimestampDecorator;
    decoratorCreators_["color"] = createColorDecorator;
    decoratorCreators_["compression"] = createCompressionDecorator;
    decoratorCreators_["filter"] = createFilterDecorator;
    decoratorCreators_["format"] = createFormatDecorator;
}

// 字符串转换函数
std::string LogOutputFactory::outputTypeToString(OutputType type) {
    switch (type) {
        case OutputType::FILE: return "file";
        case OutputType::CONSOLE: return "console";
        case OutputType::NETWORK: return "network";
        case OutputType::CUSTOM: return "custom";
        default: return "unknown";
    }
}

LogOutputFactory::OutputType LogOutputFactory::stringToOutputType(const std::string& str) {
    if (str == "file") return OutputType::FILE;
    if (str == "console") return OutputType::CONSOLE;
    if (str == "network") return OutputType::NETWORK;
    if (str == "custom") return OutputType::CUSTOM;
    return OutputType::CONSOLE; // 默认
}

std::string LogOutputFactory::decoratorTypeToString(DecoratorType type) {
    switch (type) {
        case DecoratorType::TIMESTAMP: return "timestamp";
        case DecoratorType::COLOR: return "color";
        case DecoratorType::COMPRESSION: return "compression";
        case DecoratorType::FILTER: return "filter";
        case DecoratorType::FORMAT: return "format";
        case DecoratorType::CUSTOM: return "custom";
        default: return "unknown";
    }
}

LogOutputFactory::DecoratorType LogOutputFactory::stringToDecoratorType(const std::string& str) {
    if (str == "timestamp") return DecoratorType::TIMESTAMP;
    if (str == "color") return DecoratorType::COLOR;
    if (str == "compression") return DecoratorType::COMPRESSION;
    if (str == "filter") return DecoratorType::FILTER;
    if (str == "format") return DecoratorType::FORMAT;
    if (str == "custom") return DecoratorType::CUSTOM;
    return DecoratorType::TIMESTAMP; // 默认
}

} // namespace async_log
