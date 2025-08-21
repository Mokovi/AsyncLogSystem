#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace log_system {

/**
 * @brief 线程安全队列模板类
 * 
 * 提供线程安全的队列操作，支持多生产者-多消费者模式
 * 使用互斥锁和条件变量保证线程安全
 */
template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;
    
    // 禁用拷贝构造和赋值
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;
    
    /**
     * @brief 向队列添加元素
     * @param item 要添加的元素
     */
    void push(const T& item);
    
    /**
     * @brief 从队列取出元素
     * @param item 用于存储取出元素的引用
     * @return 是否成功取出元素
     */
    bool pop(T& item);
    
    /**
     * @brief 尝试从队列取出元素（非阻塞）
     * @param item 用于存储取出元素的引用
     * @return 是否成功取出元素
     */
    bool tryPop(T& item);
    
    /**
     * @brief 检查队列是否为空
     * @return 队列是否为空
     */
    bool empty() const;
    
    /**
     * @brief 获取队列大小
     * @return 队列中元素的数量
     */
    size_t size() const;
    
    /**
     * @brief 清空队列
     */
    void clear();
    
    /**
     * @brief 等待队列非空
     * @param timeout_ms 超时时间（毫秒），0表示无限等待
     * @return 是否在超时前队列变为非空
     */
    bool waitForData(int timeout_ms = 0);

private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable condition_;
};

} // namespace log_system

// 模板实现必须在头文件中
#include "thread_safe_queue.hpp"
