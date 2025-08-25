/**
 * @file logDispatcher.hpp
 * @brief 日志分发器类
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 负责将日志消息分发到各个输出目标，支持过滤和路由
 * @note 此实现是线程安全的
 * @see LogManager, ILogOutput
 * @since 1.0.0
 */

#pragma once

#include "logTypes.hpp"
#include "logOutput.hpp"
#include <memory>
#include <vector>
#include <mutex>
#include <functional>
#include <atomic> // Added for std::atomic

namespace async_log {

/**
 * @brief 日志分发器类
 * @details 负责将日志消息分发到各个输出目标，支持消息过滤和路由
 * @note 此实现是线程安全的
 * @since 1.0.0
 */
class LogDispatcher {
private:
    std::vector<std::unique_ptr<ILogOutput>> outputs_;  ///< 输出目标列表
    mutable std::mutex outputsMutex_;                   ///< 输出列表互斥锁
    
    // 过滤和路由函数
    std::function<bool(const LogMessage&)> messageFilter_;  ///< 消息过滤器
    std::function<size_t(const LogMessage&)> routeFunction_; ///< 路由函数
    
public:
    /**
     * @brief 构造函数
     * @since 1.0.0
     */
    LogDispatcher();
    
    /**
     * @brief 析构函数
     * @since 1.0.0
     */
    ~LogDispatcher();
    
    // 禁用拷贝构造和赋值
    LogDispatcher(const LogDispatcher&) = delete;
    LogDispatcher& operator=(const LogDispatcher&) = delete;
    
    // 允许移动构造和赋值
    LogDispatcher(LogDispatcher&&) noexcept;
    LogDispatcher& operator=(LogDispatcher&&) noexcept;
    
    /**
     * @brief 分发日志消息
     * @param[in] msg 要分发的日志消息
     * @return 成功分发的输出数量
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    size_t dispatch(const LogMessage& msg);
    
    /**
     * @brief 添加输出目标
     * @param[in] output 要添加的输出目标
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void addOutput(std::unique_ptr<ILogOutput> output);
    
    /**
     * @brief 移除指定索引的输出
     * @param[in] index 要移除的输出索引
     * @return true表示成功，false表示索引无效
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    bool removeOutput(size_t index);
    
    /**
     * @brief 清空所有输出
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void clearOutputs();
    
    /**
     * @brief 获取输出数量
     * @return 当前配置的输出数量
     * @since 1.0.0
     */
    size_t getOutputCount() const;
    
    /**
     * @brief 刷新所有输出
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void flush();
    
    /**
     * @brief 关闭所有输出
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void close();
    
    // 过滤和路由配置
    /**
     * @brief 设置消息过滤器
     * @param[in] filter 过滤函数，返回true表示通过，false表示过滤
     * @since 1.0.0
     */
    void setMessageFilter(std::function<bool(const LogMessage&)> filter);
    
    /**
     * @brief 设置路由函数
     * @param[in] router 路由函数，返回目标输出的索引
     * @since 1.0.0
     */
    void setRouteFunction(std::function<size_t(const LogMessage&)> router);
    
    /**
     * @brief 清除消息过滤器
     * @since 1.0.0
     */
    void clearMessageFilter();
    
    /**
     * @brief 清除路由函数
     * @since 1.0.0
     */
    void clearRouteFunction();
    
    /**
     * @brief 设置默认路由策略
     * @param[in] strategy 路由策略：0=所有输出，1=轮询，2=随机
     * @since 1.0.0
     */
    void setDefaultRoutingStrategy(int strategy);
    
private:
    /**
     * @brief 检查消息是否应该被过滤
     * @param[in] msg 要检查的消息
     * @return true表示应该通过，false表示应该过滤
     * @since 1.0.0
     */
    bool shouldDispatch(const LogMessage& msg);
    
    /**
     * @brief 确定目标输出
     * @param[in] msg 日志消息
     * @return 目标输出的索引向量
     * @since 1.0.0
     */
    std::vector<size_t> getTargetOutputs(const LogMessage& msg);
    
    /**
     * @brief 默认路由策略
     * @param[in] msg 日志消息
     * @return 目标输出的索引向量
     * @since 1.0.0
     */
    std::vector<size_t> defaultRouting(const LogMessage& msg);
    
    /**
     * @brief 轮询路由策略
     * @param[in] msg 日志消息
     * @return 目标输出的索引向量
     * @since 1.0.0
     */
    std::vector<size_t> roundRobinRouting(const LogMessage& msg);
    
    /**
     * @brief 随机路由策略
     * @param[in] msg 日志消息
     * @return 目标输出的索引向量
     * @since 1.0.0
     */
    std::vector<size_t> randomRouting(const LogMessage& msg);
    
    int routingStrategy_;               ///< 当前路由策略
    std::atomic<size_t> roundRobinCounter_; ///< 轮询计数器
};

} // namespace async_log
