#pragma once
#include "log_system.h"
#include "thread_safe_queue.h"
#include <thread>
#include <atomic>
#include <memory>

namespace log_system {

/**
 * @brief 异步日志系统类
 * 
 * 继承自基础LogSystem，添加异步日志记录功能
 * 使用独立的工作线程处理日志消息，避免阻塞主线程
 */
class AsyncLogSystem : public LogSystem {
public:
    static AsyncLogSystem& getInstance();
    
    AsyncLogSystem();
    ~AsyncLogSystem();
    
    // 禁用拷贝构造和赋值
    AsyncLogSystem(const AsyncLogSystem&) = delete;
    AsyncLogSystem& operator=(const AsyncLogSystem&) = delete;
    
    /**
     * @brief 异步日志记录接口
     * @param level 日志级别
     * @param message 日志消息
     */
    void logAsync(LogLevel level, const std::string& message);
    
    /**
     * @brief 启动异步日志系统
     */
    void start();
    
    /**
     * @brief 停止异步日志系统
     */
    void stop();
    
    /**
     * @brief 检查系统是否正在运行
     * @return 系统是否正在运行
     */
    bool isRunning() const;
    
    /**
     * @brief 获取队列中的消息数量
     * @return 队列中的消息数量
     */
    size_t getQueueSize() const;
    
    /**
     * @brief 等待所有日志消息处理完成
     * @param timeout_ms 超时时间（毫秒），0表示无限等待
     * @return 是否在超时前处理完成
     */
    bool waitForCompletion(int timeout_ms = 0);

protected:
    // 重写基础日志记录方法，使其调用异步版本
    void log(LogLevel level, const std::string& message) override;

private:
    /**
     * @brief 工作线程函数
     * 持续从队列中取出日志消息并处理
     */
    void workerFunction();
    
    /**
     * @brief 处理单条日志消息
     * @param msg 日志消息
     */
    void processMessage(const LogMessage& msg);

private:
    std::unique_ptr<ThreadSafeQueue<LogMessage>> messageQueue_;
    std::thread workerThread_;
    std::atomic<bool> running_;
    std::atomic<bool> initialized_;
    
    // 配置相关
    size_t maxQueueSize_;
    int workerTimeoutMs_;
};

} // namespace log_system
