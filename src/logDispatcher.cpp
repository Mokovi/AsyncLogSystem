/**
 * @file logDispatcher.cpp
 * @brief 日志分发器实现
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现日志消息的分发、过滤和路由功能
 * @see log_dispatcher.hpp
 * @since 1.0.0
 */

#include "logDispatcher.hpp"
#include "logTypes.hpp"
#include <algorithm>
#include <random>
#include <chrono>

namespace async_log {

LogDispatcher::LogDispatcher()
    : routingStrategy_(0), roundRobinCounter_(0) {
}

LogDispatcher::~LogDispatcher() = default;

LogDispatcher::LogDispatcher(LogDispatcher&& other) noexcept
    : outputs_(std::move(other.outputs_)),
      messageFilter_(std::move(other.messageFilter_)),
      routeFunction_(std::move(other.routeFunction_)),
      routingStrategy_(other.routingStrategy_),
      roundRobinCounter_(other.roundRobinCounter_.load()) {
}

LogDispatcher& LogDispatcher::operator=(LogDispatcher&& other) noexcept {
    if (this != &other) {
        outputs_ = std::move(other.outputs_);
        messageFilter_ = std::move(other.messageFilter_);
        routeFunction_ = std::move(other.routeFunction_);
        routingStrategy_ = other.routingStrategy_;
        roundRobinCounter_ = other.roundRobinCounter_.load();
    }
    return *this;
}

size_t LogDispatcher::dispatch(const LogMessage& msg) {
    if (!shouldDispatch(msg)) {
        return 0;
    }
    
    std::vector<size_t> targetOutputs = getTargetOutputs(msg);
    size_t successCount = 0;
    
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    for (size_t index : targetOutputs) {
        if (index < outputs_.size() && outputs_[index] && outputs_[index]->isAvailable()) {
            try {
                outputs_[index]->write(msg);
                successCount++;
            } catch (const std::exception&) {
                // 忽略输出错误，继续处理其他输出
            }
        }
    }
    
    return successCount;
}

void LogDispatcher::addOutput(std::unique_ptr<ILogOutput> output) {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    outputs_.push_back(std::move(output));
}

bool LogDispatcher::removeOutput(size_t index) {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    if (index >= outputs_.size()) {
        return false;
    }
    
    outputs_.erase(outputs_.begin() + index);
    return true;
}

void LogDispatcher::clearOutputs() {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    outputs_.clear();
}

size_t LogDispatcher::getOutputCount() const {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    return outputs_.size();
}

void LogDispatcher::flush() {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    for (auto& output : outputs_) {
        if (output && output->isAvailable()) {
            try {
                output->flush();
            } catch (const std::exception&) {
                // 忽略flush错误
            }
        }
    }
}

void LogDispatcher::close() {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    for (auto& output : outputs_) {
        if (output) {
            try {
                output->close();
            } catch (const std::exception&) {
                // 忽略关闭错误
            }
        }
    }
}

void LogDispatcher::setMessageFilter(std::function<bool(const LogMessage&)> filter) {
    messageFilter_ = std::move(filter);
}

void LogDispatcher::setRouteFunction(std::function<size_t(const LogMessage&)> router) {
    routeFunction_ = std::move(router);
}

void LogDispatcher::clearMessageFilter() {
    messageFilter_ = nullptr;
}

void LogDispatcher::clearRouteFunction() {
    routeFunction_ = nullptr;
}

void LogDispatcher::setDefaultRoutingStrategy(int strategy) {
    routingStrategy_ = strategy;
}

bool LogDispatcher::shouldDispatch(const LogMessage& msg) {
    if (messageFilter_) {
        return messageFilter_(msg);
    }
    return true;
}

std::vector<size_t> LogDispatcher::getTargetOutputs(const LogMessage& msg) {
    if (routeFunction_) {
        size_t targetIndex = routeFunction_(msg);
        if (targetIndex < outputs_.size()) {
            return {targetIndex};
        }
        return {};
    }
    
    return defaultRouting(msg);
}

std::vector<size_t> LogDispatcher::defaultRouting(const LogMessage& msg) {
    switch (routingStrategy_) {
        case 1: return roundRobinRouting(msg);
        case 2: return randomRouting(msg);
        default: {
            std::vector<size_t> indices;
            std::lock_guard<std::mutex> lock(outputsMutex_);
            
            for (size_t i = 0; i < outputs_.size(); ++i) {
                indices.push_back(i);
            }
            
            return indices;
        }
    }
}

std::vector<size_t> LogDispatcher::roundRobinRouting(const LogMessage& msg) {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    if (outputs_.empty()) {
        return {};
    }
    
    size_t index = roundRobinCounter_++ % outputs_.size();
    return {index};
}

std::vector<size_t> LogDispatcher::randomRouting(const LogMessage& msg) {
    std::lock_guard<std::mutex> lock(outputsMutex_);
    
    if (outputs_.empty()) {
        return {};
    }
    
    // 使用当前时间作为种子
    auto now = std::chrono::high_resolution_clock::now();
    auto seed = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();
    
    std::mt19937 gen(static_cast<unsigned int>(seed));
    std::uniform_int_distribution<size_t> dis(0, outputs_.size() - 1);
    
    size_t index = dis(gen);
    return {index};
}

} // namespace async_log
