/**
 * @file lockFreeQueue.hpp
 * @brief 无锁队列模板类
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现了一个线程安全的无锁队列，支持多生产者多消费者场景
 * @note 此实现使用原子操作保证线程安全，适用于高性能日志系统
 * @see LogManager, LogMessage
 * @since 1.0.0
 */

#pragma once

#include <atomic>
#include <memory>
#include <vector>
#include <cstddef>

namespace async_log {

/**
 * @brief 无锁队列节点结构
 * @details 队列中的每个节点，包含数据和指向下一个节点的指针
 * @tparam T 节点数据类型
 * @since 1.0.0
 */
template<typename T>
struct QueueNode {
    T data;                              ///< 节点数据
    std::atomic<QueueNode*> next;        ///< 指向下一个节点的原子指针
    
    /**
     * @brief 构造函数
     * @param[in] item 要存储的数据
     * @since 1.0.0
     */
    explicit QueueNode(const T& item) : data(item), next(nullptr) {}
    
    /**
     * @brief 移动构造函数
     * @param[in] item 要移动的数据
     * @since 1.0.0
     */
    explicit QueueNode(T&& item) : data(std::move(item)), next(nullptr) {}
};

/**
 * @brief 无锁队列模板类
 * @details 实现了基于链表的无锁队列，支持多生产者多消费者
 * @note 此实现是线程安全的，但要求类型T支持拷贝构造和移动构造
 * @tparam T 队列中存储的数据类型
 * @since 1.0.0
 */
template<typename T>
class LockFreeQueue {
private:
    std::atomic<QueueNode<T>*> head_;    ///< 队列头指针
    std::atomic<QueueNode<T>*> tail_;    ///< 队列尾指针
    std::atomic<size_t> size_;           ///< 队列大小
    
public:
    /**
     * @brief 构造函数
     * @since 1.0.0
     */
    LockFreeQueue();
    
    /**
     * @brief 析构函数
     * @since 1.0.0
     */
    ~LockFreeQueue();
    
    // 禁用拷贝构造和赋值
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;
    
    // 允许移动构造和赋值
    LockFreeQueue(LockFreeQueue&&) noexcept;
    LockFreeQueue& operator=(LockFreeQueue&&) noexcept;
    
    /**
     * @brief 向队列尾部添加元素
     * @param[in] item 要添加的元素
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void push(const T& item);
    
    /**
     * @brief 向队列尾部添加元素（移动语义）
     * @param[in] item 要移动的元素
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void push(T&& item);
    
    /**
     * @brief 从队列头部取出元素
     * @param[out] item 存储取出的元素
     * @return true表示成功取出，false表示队列为空
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    bool pop(T& item);
    
    /**
     * @brief 检查队列是否为空
     * @return true表示队列为空，false表示队列非空
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    bool empty() const;
    
    /**
     * @brief 获取队列大小
     * @return 队列中元素的数量
     * @note 此操作是线程安全的，但返回的值可能不是精确的
     * @since 1.0.0
     */
    size_t getSize() const;
    
    /**
     * @brief 批量添加元素
     * @param[in] items 要添加的元素向量
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void pushBatch(const std::vector<T>& items);
    
    /**
     * @brief 批量取出元素
     * @param[out] items 存储取出的元素
     * @param[in] maxCount 最大取出数量
     * @return 实际取出的元素数量
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    size_t popBatch(std::vector<T>& items, size_t maxCount);
    
    /**
     * @brief 清空队列
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void clear();
    
private:
    /**
     * @brief 清理资源
     * @since 1.0.0
     */
    void cleanup();
    
    /**
     * @brief 创建哨兵节点
     * @return 指向哨兵节点的指针
     * @since 1.0.0
     */
    QueueNode<T>* createSentinel();
};

// 模板类实现
template<typename T>
LockFreeQueue<T>::LockFreeQueue() : head_(nullptr), tail_(nullptr), size_(0) {
    // 创建哨兵节点
    QueueNode<T>* sentinel = createSentinel();
    head_.store(sentinel);
    tail_.store(sentinel);
}

template<typename T>
LockFreeQueue<T>::~LockFreeQueue() {
    cleanup();
}

template<typename T>
LockFreeQueue<T>::LockFreeQueue(LockFreeQueue&& other) noexcept 
    : head_(other.head_.load()), tail_(other.tail_.load()), size_(other.size_.load()) {
    other.head_.store(nullptr);
    other.tail_.store(nullptr);
    other.size_.store(0);
}

template<typename T>
LockFreeQueue<T>& LockFreeQueue<T>::operator=(LockFreeQueue&& other) noexcept {
    if (this != &other) {
        cleanup();
        head_.store(other.head_.load());
        tail_.store(other.tail_.load());
        size_.store(other.size_.load());
        other.head_.store(nullptr);
        other.tail_.store(nullptr);
        other.size_.store(0);
    }
    return *this;
}

template<typename T>
void LockFreeQueue<T>::push(const T& item) {
    QueueNode<T>* newNode = new QueueNode<T>(item);
    
    QueueNode<T>* oldTail = tail_.load();
    QueueNode<T>* expected = oldTail;
    
    // 尝试更新尾指针
    while (!tail_.compare_exchange_weak(expected, newNode)) {
        oldTail = expected;
        expected = oldTail;
    }
    
    // 更新前一个节点的next指针
    oldTail->next.store(newNode);
    size_.fetch_add(1);
}

template<typename T>
void LockFreeQueue<T>::push(T&& item) {
    QueueNode<T>* newNode = new QueueNode<T>(std::move(item));
    
    QueueNode<T>* oldTail = tail_.load();
    QueueNode<T>* expected = oldTail;
    
    // 尝试更新尾指针
    while (!tail_.compare_exchange_weak(expected, newNode)) {
        oldTail = expected;
        expected = oldTail;
    }
    
    // 更新前一个节点的next指针
    oldTail->next.store(newNode);
    size_.fetch_add(1);
}

template<typename T>
bool LockFreeQueue<T>::pop(T& item) {
    QueueNode<T>* oldHead = head_.load();
    QueueNode<T>* oldTail = tail_.load();
    
    // 检查队列是否为空
    if (oldHead == oldTail) {
        return false;
    }
    
    QueueNode<T>* next = oldHead->next.load();
    if (next == nullptr) {
        return false;
    }
    
    // 尝试更新头指针
    if (head_.compare_exchange_strong(oldHead, next)) {
        item = std::move(next->data);
        delete oldHead;
        size_.fetch_sub(1);
        return true;
    }
    
    return false;
}

template<typename T>
bool LockFreeQueue<T>::empty() const {
    return head_.load() == tail_.load();
}

template<typename T>
size_t LockFreeQueue<T>::getSize() const {
    return size_.load();
}

template<typename T>
void LockFreeQueue<T>::pushBatch(const std::vector<T>& items) {
    for (const auto& item : items) {
        push(item);
    }
}

template<typename T>
size_t LockFreeQueue<T>::popBatch(std::vector<T>& items, size_t maxCount) {
    items.clear();
    items.reserve(maxCount);
    
    size_t count = 0;
    T item;
    
    while (count < maxCount && pop(item)) {
        items.push_back(std::move(item));
        count++;
    }
    
    return count;
}

template<typename T>
void LockFreeQueue<T>::clear() {
    T item;
    while (pop(item)) {
        // 清空所有元素
    }
}

template<typename T>
void LockFreeQueue<T>::cleanup() {
    T item;
    while (pop(item)) {
        // 清理所有元素
    }
    
    // 删除哨兵节点
    QueueNode<T>* sentinel = head_.load();
    if (sentinel) {
        delete sentinel;
    }
}

template<typename T>
QueueNode<T>* LockFreeQueue<T>::createSentinel() {
    return new QueueNode<T>(T{});
}

} // namespace async_log
