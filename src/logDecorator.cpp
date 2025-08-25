/**
 * @file logDecorator.cpp
 * @brief 日志装饰器实现
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现各种日志装饰器，为日志输出添加额外功能
 * @see log_decorator.hpp
 * @since 1.0.0
 */

#include "logDecorator.hpp"
#include "logTypes.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <regex>
#include <algorithm>

namespace async_log {

// LogDecorator 实现
LogDecorator::LogDecorator(std::unique_ptr<ILogOutput> output)
    : wrapped_(std::move(output)) {
}

void LogDecorator::write(const LogMessage& msg) {
    if (wrapped_) {
        wrapped_->write(msg);
    }
}

void LogDecorator::flush() {
    if (wrapped_) {
        wrapped_->flush();
    }
}

void LogDecorator::close() {
    if (wrapped_) {
        wrapped_->close();
    }
}

bool LogDecorator::isAvailable() const {
    return wrapped_ && wrapped_->isAvailable();
}

ILogOutput* LogDecorator::getWrappedOutput() const {
    return wrapped_.get();
}

void LogDecorator::setWrappedOutput(std::unique_ptr<ILogOutput> output) {
    wrapped_ = std::move(output);
}

// TimestampDecorator 实现
TimestampDecorator::TimestampDecorator(std::unique_ptr<ILogOutput> output, 
                                     const std::string& timeFormat)
    : LogDecorator(std::move(output)), format_(timeFormat) {
}

void TimestampDecorator::write(const LogMessage& msg) {
    if (wrapped_) {
        // 创建带时间戳的消息副本
        LogMessage decoratedMsg = msg;
        std::string timestamp = getCurrentTimestamp();
        
        // 在消息前添加时间戳
        decoratedMsg.message = "[" + timestamp + "] " + msg.message;
        
        wrapped_->write(decoratedMsg);
    }
}

void TimestampDecorator::setTimeFormat(const std::string& format) {
    format_ = format;
}

std::string TimestampDecorator::getTimeFormat() const {
    return format_;
}

std::string TimestampDecorator::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return formatTime(now);
}

std::string TimestampDecorator::formatTime(const std::chrono::system_clock::time_point& timePoint) {
    auto time_t = std::chrono::system_clock::to_time_t(timePoint);
    auto tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, format_.c_str());
    return oss.str();
}

// ColorDecorator 实现
ColorDecorator::ColorDecorator(std::unique_ptr<ILogOutput> output, bool enableColor)
    : LogDecorator(std::move(output)), enableColor_(enableColor) {
}

void ColorDecorator::write(const LogMessage& msg) {
    if (wrapped_) {
        if (enableColor_) {
            // 创建带颜色的消息副本
            LogMessage coloredMsg = msg;
            std::string colorCode = getColorCode(msg.level);
            std::string resetCode = getResetCode();
            
            // 在消息前后添加颜色代码
            coloredMsg.message = colorCode + msg.message + resetCode;
            
            wrapped_->write(coloredMsg);
        } else {
            wrapped_->write(msg);
        }
    }
}

void ColorDecorator::setColorEnabled(bool enable) {
    enableColor_ = enable;
}

bool ColorDecorator::isColorEnabled() const {
    return enableColor_;
}

std::string ColorDecorator::getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "\033[36m"; // 青色
        case LogLevel::INFO:  return "\033[32m"; // 绿色
        case LogLevel::WARN:  return "\033[33m"; // 黄色
        case LogLevel::ERROR: return "\033[31m"; // 红色
        case LogLevel::FATAL: return "\033[35m"; // 紫色
        default:              return "\033[0m";  // 默认
    }
}

std::string ColorDecorator::getResetCode() {
    return "\033[0m";
}

// CompressionDecorator 实现
CompressionDecorator::CompressionDecorator(std::unique_ptr<ILogOutput> output, 
                                         bool enableCompression, size_t minSize)
    : LogDecorator(std::move(output)), enableCompression_(enableCompression), minSize_(minSize) {
}

void CompressionDecorator::write(const LogMessage& msg) {
    if (wrapped_) {
        if (enableCompression_ && msg.message.length() >= minSize_) {
            // 创建压缩消息
            LogMessage compressedMsg = msg;
            compressedMsg.message = compress(msg.message);
            
            wrapped_->write(compressedMsg);
        } else {
            wrapped_->write(msg);
        }
    }
}

void CompressionDecorator::setCompressionEnabled(bool enable) {
    enableCompression_ = enable;
}

void CompressionDecorator::setMinCompressionSize(size_t minSize) {
    minSize_ = minSize;
}

std::string CompressionDecorator::compress(const std::string& data) {
    // 简单的压缩实现：移除多余空格和换行
    std::string compressed = data;
    
    // 移除多余空格
    std::regex multipleSpaces("\\s+");
    compressed = std::regex_replace(compressed, multipleSpaces, " ");
    
    // 移除首尾空格
    compressed.erase(0, compressed.find_first_not_of(" \t\n\r"));
    compressed.erase(compressed.find_last_not_of(" \t\n\r") + 1);
    
    return "[COMPRESSED] " + compressed;
}

std::string CompressionDecorator::decompress(const std::string& data) {
    // 简单的解压实现
    if (data.substr(0, 13) == "[COMPRESSED] ") {
        return data.substr(13);
    }
    return data;
}

// FilterDecorator 实现
FilterDecorator::FilterDecorator(std::unique_ptr<ILogOutput> output, 
                               std::function<bool(const LogMessage&)> filter)
    : LogDecorator(std::move(output)), filter_(std::move(filter)) {
}

void FilterDecorator::write(const LogMessage& msg) {
    if (wrapped_ && shouldPass(msg)) {
        wrapped_->write(msg);
    }
}

void FilterDecorator::setFilter(std::function<bool(const LogMessage&)> filter) {
    filter_ = std::move(filter);
}

void FilterDecorator::clearFilter() {
    filter_ = nullptr;
}

bool FilterDecorator::shouldPass(const LogMessage& msg) {
    if (filter_) {
        return filter_(msg);
    }
    return true; // 没有过滤器时默认通过
}

// FormatDecorator 实现
FormatDecorator::FormatDecorator(std::unique_ptr<ILogOutput> output, const std::string& format)
    : LogDecorator(std::move(output)), format_(format) {
}

void FormatDecorator::write(const LogMessage& msg) {
    if (wrapped_) {
        // 创建格式化消息
        LogMessage formattedMsg = msg;
        formattedMsg.message = formatMessage(msg);
        
        wrapped_->write(formattedMsg);
    }
}

void FormatDecorator::setFormat(const std::string& format) {
    format_ = format;
}

std::string FormatDecorator::getFormat() const {
    return format_;
}

std::string FormatDecorator::formatMessage(const LogMessage& msg) {
    return replacePlaceholders(format_, msg);
}

std::string FormatDecorator::replacePlaceholders(const std::string& format, const LogMessage& msg) {
    std::string result = format;
    
    // 替换各种占位符
    std::map<std::string, std::string> replacements = {
        {"{level}", levelToString(msg.level)},
        {"{message}", msg.message},
        {"{file}", msg.file},
        {"{line}", std::to_string(msg.line)},
        {"{function}", msg.function},
        {"{time}", std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
            msg.timestamp.time_since_epoch()).count())},
        {"{thread}", std::to_string(std::hash<std::thread::id>{}(msg.threadId))}
    };
    
    for (const auto& [placeholder, value] : replacements) {
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos = result.find(placeholder, pos + value.length());
        }
    }
    
    return result;
}

} // namespace async_log
