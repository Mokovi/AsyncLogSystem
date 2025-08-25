/**
 * @file logManager.cpp
 * @brief 日志管理器实现
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现日志管理器的核心功能，包括单例管理、消息处理、线程管理等
 * @see log_manager.hpp
 * @since 1.0.0
 */

#include "logManager.hpp"
#include "logDispatcher.hpp"
#include "logFactory.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>

namespace async_log {

// 静态成员初始化
std::unique_ptr<LogManager> LogManager::instance_;
std::mutex LogManager::instanceMutex_;

LogManager& LogManager::getInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    
    if (!instance_) {
        instance_ = std::unique_ptr<LogManager>(new LogManager());
    }
    
    return *instance_;
}

void LogManager::destroyInstance() {
    std::lock_guard<std::mutex> lock(instanceMutex_);
    
    if (instance_) {
        instance_->stop();
        instance_.reset();
    }
}

LogManager::LogManager()
    : running_(false), shouldStop_(false) {
    
    initializeDefaultConfig();
    createDefaultOutputs();
    
    // 创建核心组件
    messageQueue_ = std::make_unique<LockFreeQueue<LogMessage>>();
    dispatcher_ = std::make_unique<LogDispatcher>();
}

LogManager::~LogManager() {
    stop();
}

void LogManager::setConfig(const LogConfig& config) {
    std::lock_guard<std::mutex> lock(configMutex_);
    config_ = std::make_unique<LogConfig>(config);
}

LogConfig LogManager::getConfig() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    return config_ ? *config_ : LogConfig{};
}

bool LogManager::loadConfigFromFile(const std::string& configFile) {
    // TODO: 实现从文件加载配置的功能
    // 这里暂时返回false
    return false;
}

void LogManager::addOutput(std::unique_ptr<ILogOutput> output) {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    if (output) {
        outputs_.push_back(std::move(output));
        
        // 同时添加到分发器
        if (dispatcher_) {
            // 创建一个新的副本添加到分发器
            auto outputCopy = std::make_unique<ConsoleOutput>(true); // 临时使用ConsoleOutput作为示例
            dispatcher_->addOutput(std::move(outputCopy));
        }
    }
}

bool LogManager::removeOutput(size_t index) {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    if (index >= outputs_.size()) {
        return false;
    }
    
    // 从dispatcher中移除对应索引的输出
    if (dispatcher_) {
        dispatcher_->removeOutput(index);
    }
    
    outputs_.erase(outputs_.begin() + index);
    return true;
}

void LogManager::clearOutputs() {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    outputs_.clear();
    
    if (dispatcher_) {
        dispatcher_->clearOutputs();
    }
}

size_t LogManager::getOutputCount() const {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    return outputs_.size();
}

void LogManager::log(LogLevel level, const std::string& message) {
    if (!shouldLog(level)) {
        return;
    }
    
    LogMessage msg(level, message);
    messageQueue_->push(std::move(msg));
}

void LogManager::log(LogLevel level, const std::string& message, 
                     const std::string& file, int line, const std::string& function) {
    if (!shouldLog(level)) {
        return;
    }
    
    LogMessage msg(level, message, file, line, function);
    messageQueue_->push(std::move(msg));
}

void LogManager::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void LogManager::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void LogManager::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void LogManager::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void LogManager::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

bool LogManager::start() {
    if (running_.load()) {
        return true; // 已经在运行
    }
    
    shouldStop_ = false;
    running_ = true;
    
    // 启动工作线程
    workerThread_ = std::thread(&LogManager::workerFunction, this);
    
    return true;
}

void LogManager::stop() {
    if (!running_.load()) {
        return; // 已经停止
    }
    
    shouldStop_ = true;
    running_ = false;
    
    // 通知工作线程
    workerCondition_.notify_all();
    
    // 等待工作线程结束
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
    
    // 刷新所有输出
    flush();
}

void LogManager::flush() {
    if (dispatcher_) {
        dispatcher_->flush();
    }
    
    // 等待队列中的消息处理完成
    while (!messageQueue_->empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool LogManager::isRunning() const {
    return running_.load();
}

size_t LogManager::getQueueSize() const {
    return messageQueue_ ? messageQueue_->getSize() : 0;
}

void LogManager::workerFunction() {
    std::vector<LogMessage> messages;
    const size_t batchSize = 100; // 批量处理大小
    
    while (!shouldStop_.load()) {
        // 批量取出消息
        size_t count = messageQueue_->popBatch(messages, batchSize);
        
        if (count > 0) {
            // 处理消息
            for (const auto& msg : messages) {
                processMessage(msg);
            }
            messages.clear();
        } else {
            // 没有消息时等待
            std::unique_lock<std::mutex> lock(configMutex_);
            workerCondition_.wait_for(lock, std::chrono::milliseconds(100));
        }
    }
    
    // 处理剩余消息
    while (messageQueue_->popBatch(messages, batchSize) > 0) {
        for (const auto& msg : messages) {
            processMessage(msg);
        }
        messages.clear();
    }
}

void LogManager::processMessage(const LogMessage& msg) {
    if (dispatcher_) {
        dispatcher_->dispatch(msg);
    }
}

void LogManager::initializeDefaultConfig() {
    config_ = std::make_unique<LogConfig>();
    // 使用LogConfig的默认值
}

void LogManager::createDefaultOutputs() {
    // 创建默认的控制台输出
    auto consoleOutput = std::make_unique<ConsoleOutput>(true);
    
    // 添加时间戳装饰器
    auto timestampDecorator = std::make_unique<TimestampDecorator>(std::move(consoleOutput));
    
    // 添加颜色装饰器
    auto colorDecorator = std::make_unique<ColorDecorator>(std::move(timestampDecorator));
    
    // 添加到输出列表
    outputs_.push_back(std::move(colorDecorator));
    
    // 同时添加到分发器
    if (dispatcher_) {
        // 创建一个新的副本添加到分发器
        auto consoleOutput2 = std::make_unique<ConsoleOutput>(true);
        auto timestampDecorator2 = std::make_unique<TimestampDecorator>(std::move(consoleOutput2));
        auto colorDecorator2 = std::make_unique<ColorDecorator>(std::move(timestampDecorator2));
        dispatcher_->addOutput(std::move(colorDecorator2));
    }
}

bool LogManager::shouldLog(LogLevel level) const {
    std::lock_guard<std::mutex> lock(configMutex_);
    
    if (!config_) {
        return true; // 没有配置时默认记录所有日志
    }
    
    return static_cast<int>(level) >= static_cast<int>(config_->minLevel);
}

} // namespace async_log
