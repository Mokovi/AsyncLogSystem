#include "async_log_system.h"
#include <iostream>
#include <chrono>

namespace log_system {

AsyncLogSystem& AsyncLogSystem::getInstance() {
    static AsyncLogSystem instance;
    return instance;
}

AsyncLogSystem::AsyncLogSystem()
    : messageQueue_(std::make_unique<ThreadSafeQueue<LogMessage>>())
    , running_(false)
    , initialized_(false)
    , maxQueueSize_(10000)  // 默认最大队列大小
    , workerTimeoutMs_(100)  // 默认工作线程超时时间
{
}

AsyncLogSystem::~AsyncLogSystem() {
    stop();
}

void AsyncLogSystem::start() {
    if (initialized_ || running_) {
        return;
    }
    
    running_ = true;
    initialized_ = true;
    
    // 启动工作线程
    workerThread_ = std::thread(&AsyncLogSystem::workerFunction, this);
    
    std::cout << "[AsyncLogSystem] 异步日志系统已启动" << std::endl;
}

void AsyncLogSystem::stop() {
    if (!running_) {
        return;
    }
    
    running_ = false;
    
    // 等待工作线程完成
    if (workerThread_.joinable()) {
        workerThread_.join();
    }
    
    initialized_ = false;
    
    std::cout << "[AsyncLogSystem] 异步日志系统已停止" << std::endl;
}

void AsyncLogSystem::logAsync(LogLevel level, const std::string& message) {
    if (!running_) {
        // 如果异步系统未启动，回退到同步模式
        LogSystem::log(level, message);
        return;
    }
    
    // 检查队列大小限制
    if (messageQueue_->size() >= maxQueueSize_) {
        std::cerr << "[AsyncLogSystem] 警告：队列已满，丢弃日志消息: " << message << std::endl;
        return;
    }
    
    // 创建日志消息并加入队列
    LogMessage msg(level, message);
    messageQueue_->push(msg);
}

void AsyncLogSystem::log(LogLevel level, const std::string& message) {
    // 重写基础日志方法，使其调用异步版本
    logAsync(level, message);
}

void AsyncLogSystem::workerFunction() {
    LogMessage msg(LogLevel::INFO, "");
    
    while (running_) {
        // 尝试从队列中取出消息
        if (messageQueue_->tryPop(msg)) {
            processMessage(msg);
        } else {
            // 队列为空，短暂休眠避免忙等待
            std::this_thread::sleep_for(std::chrono::milliseconds(workerTimeoutMs_));
        }
    }
    
    // 处理剩余的消息
    while (messageQueue_->tryPop(msg)) {
        processMessage(msg);
    }
}

void AsyncLogSystem::processMessage(const LogMessage& msg) {
    // 调用基类的输出方法
    writeToConsole(msg);
}

bool AsyncLogSystem::isRunning() const {
    return running_;
}

size_t AsyncLogSystem::getQueueSize() const {
    return messageQueue_->size();
}

bool AsyncLogSystem::waitForCompletion(int timeout_ms) {
    if (!running_) {
        return true;
    }
    
    auto start = std::chrono::steady_clock::now();
    
    while (messageQueue_->size() > 0) {
        if (timeout_ms > 0) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            
            if (elapsed.count() >= timeout_ms) {
                return false;  // 超时
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    return true;
}

} // namespace log_system
