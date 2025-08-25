/**
 * @file logManager.hpp
 * @brief 日志管理器主类
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现日志系统的核心管理器，负责协调各个组件，提供统一的日志接口
 * @note 此实现使用单例模式，确保全局只有一个日志管理器实例
 * @see ILogOutput, LockFreeQueue, LogConfig
 * @since 1.0.0
 */

#pragma once

#include "logTypes.hpp"
#include "logOutput.hpp"
#include "lockFreeQueue.hpp"
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <functional>
#include <condition_variable>

namespace async_log {

// 前向声明
class LogDispatcher;

/**
 * @brief 日志管理器类
 * @details 负责管理整个日志系统的生命周期，协调各个组件工作
 * @note 此实现使用单例模式，线程安全
 * @since 1.0.0
 */
class LogManager {
private:
    // 单例相关
    static std::unique_ptr<LogManager> instance_;
    static std::mutex instanceMutex_;
    
    // 核心组件
    std::unique_ptr<LogConfig> config_;
    std::unique_ptr<LockFreeQueue<LogMessage>> messageQueue_;
    std::unique_ptr<LogDispatcher> dispatcher_;
    std::vector<std::unique_ptr<ILogOutput>> outputs_;
    
    // 工作线程
    std::thread workerThread_;
    std::atomic<bool> running_;
    std::atomic<bool> shouldStop_;
    
    // 同步和状态
    mutable std::mutex configMutex_;
    mutable std::mutex outputsMutex_;
    std::condition_variable workerCondition_;
    
public:
    /**
     * @brief 获取日志管理器单例实例
     * @return 日志管理器引用
     * @note 此函数是线程安全的
     * @since 1.0.0
     */
    static LogManager& getInstance();
    
    /**
     * @brief 销毁日志管理器实例
     * @note 此函数应该在程序退出前调用
     * @since 1.0.0
     */
    static void destroyInstance();
    
    // 禁用拷贝构造和赋值
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    
    // 禁止移动构造和赋值
    LogManager(LogManager&&) = delete;
    LogManager& operator=(LogManager&&) = delete;
    
    /**
     * @brief 析构函数
     * @since 1.0.0
     */
    ~LogManager();
    
    // 配置管理
    /**
     * @brief 设置日志配置
     * @param[in] config 新的配置对象
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void setConfig(const LogConfig& config);
    
    /**
     * @brief 获取当前配置
     * @return 当前配置的副本
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    LogConfig getConfig() const;
    
    /**
     * @brief 从文件加载配置
     * @param[in] configFile 配置文件路径
     * @return true表示成功，false表示失败
     * @since 1.0.0
     */
    bool loadConfigFromFile(const std::string& configFile);
    
    // 输出管理
    /**
     * @brief 添加日志输出
     * @param[in] output 要添加的输出对象
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
    
    // 日志记录
    /**
     * @brief 记录日志消息
     * @param[in] level 日志级别
     * @param[in] message 日志消息
     * @note 此操作是线程安全的，异步执行
     * @since 1.0.0
     */
    void log(LogLevel level, const std::string& message);
    
    /**
     * @brief 记录带位置信息的日志消息
     * @param[in] level 日志级别
     * @param[in] message 日志消息
     * @param[in] file 源文件名
     * @param[in] line 源文件行号
     * @param[in] function 函数名
     * @note 此操作是线程安全的，异步执行
     * @since 1.0.0
     */
    void log(LogLevel level, const std::string& message, 
             const std::string& file, int line, const std::string& function = "");
    
    // 便捷日志方法
    /**
     * @brief 记录DEBUG级别日志
     * @param[in] message 日志消息
     * @since 1.0.0
     */
    void debug(const std::string& message);
    
    /**
     * @brief 记录INFO级别日志
     * @param[in] message 日志消息
     * @since 1.0.0
     */
    void info(const std::string& message);
    
    /**
     * @brief 记录WARN级别日志
     * @param[in] message 日志消息
     * @since 1.0.0
     */
    void warn(const std::string& message);
    
    /**
     * @brief 记录ERROR级别日志
     * @param[in] message 日志消息
     * @since 1.0.0
     */
    void error(const std::string& message);
    
    /**
     * @brief 记录FATAL级别日志
     * @param[in] message 日志消息
     * @since 1.0.0
     */
    void fatal(const std::string& message);
    
    // 生命周期管理
    /**
     * @brief 启动日志系统
     * @return true表示成功，false表示失败
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    bool start();
    
    /**
     * @brief 停止日志系统
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void stop();
    
    /**
     * @brief 刷新所有输出
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    void flush();
    
    /**
     * @brief 检查系统是否运行中
     * @return true表示运行中，false表示已停止
     * @since 1.0.0
     */
    bool isRunning() const;
    
    /**
     * @brief 获取队列状态
     * @return 当前队列中的消息数量
     * @since 1.0.0
     */
    size_t getQueueSize() const;
    
private:
    /**
     * @brief 私有构造函数
     * @since 1.0.0
     */
    LogManager();
    
    /**
     * @brief 工作线程函数
     * @since 1.0.0
     */
    void workerFunction();
    
    /**
     * @brief 处理日志消息
     * @param[in] msg 要处理的日志消息
     * @since 1.0.0
     */
    void processMessage(const LogMessage& msg);
    
    /**
     * @brief 初始化默认配置
     * @since 1.0.0
     */
    void initializeDefaultConfig();
    
    /**
     * @brief 创建默认输出
     * @since 1.0.0
     */
    void createDefaultOutputs();
    
    /**
     * @brief 检查日志级别是否应该输出
     * @param[in] level 要检查的日志级别
     * @return true表示应该输出，false表示不应该输出
     * @since 1.0.0
     */
    bool shouldLog(LogLevel level) const;
};

// 全局日志宏定义
#define LOG_DEBUG(msg) async_log::LogManager::getInstance().debug(msg)
#define LOG_INFO(msg) async_log::LogManager::getInstance().info(msg)
#define LOG_WARN(msg) async_log::LogManager::getInstance().warn(msg)
#define LOG_ERROR(msg) async_log::LogManager::getInstance().error(msg)
#define LOG_FATAL(msg) async_log::LogManager::getInstance().fatal(msg)

#define LOG_DEBUG_F(msg, file, line) async_log::LogManager::getInstance().log(async_log::LogLevel::DEBUG, msg, file, line)
#define LOG_INFO_F(msg, file, line) async_log::LogManager::getInstance().log(async_log::LogLevel::INFO, msg, file, line)
#define LOG_WARN_F(msg, file, line) async_log::LogManager::getInstance().log(async_log::LogLevel::WARN, msg, file, line)
#define LOG_ERROR_F(msg, file, line) async_log::LogManager::getInstance().log(async_log::LogLevel::ERROR, msg, file, line)
#define LOG_FATAL_F(msg, file, line) async_log::LogManager::getInstance().log(async_log::LogLevel::FATAL, msg, file, line)

// 带函数名的日志宏
#define LOG_DEBUG_FUNC(msg) async_log::LogManager::getInstance().log(async_log::LogLevel::DEBUG, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO_FUNC(msg) async_log::LogManager::getInstance().log(async_log::LogLevel::INFO, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARN_FUNC(msg) async_log::LogManager::getInstance().log(async_log::LogLevel::WARN, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR_FUNC(msg) async_log::LogManager::getInstance().log(async_log::LogLevel::ERROR, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_FATAL_FUNC(msg) async_log::LogManager::getInstance().log(async_log::LogLevel::FATAL, msg, __FILE__, __LINE__, __FUNCTION__)

} // namespace async_log
