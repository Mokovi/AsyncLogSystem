/**
 * @file logOutput.hpp
 * @brief 日志输出接口定义
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 定义日志输出的抽象接口和具体实现，支持文件、控制台、网络等多种输出方式
 * @see LogManager, LogDecorator
 * @since 1.0.0
 */

#pragma once

#include "logTypes.hpp"
#include <memory>
#include <string>
#include <fstream>
#include <mutex>

namespace async_log {

/**
 * @brief 日志输出接口
 * @details 定义了日志输出的基本操作，所有具体的输出实现都必须实现此接口
 * @note 此接口是线程安全的
 * @since 1.0.0
 */
class ILogOutput {
public:
    virtual ~ILogOutput() = default;
    
    /**
     * @brief 写入日志消息
     * @param[in] msg 要写入的日志消息
     * @note 此函数应该是线程安全的
     * @since 1.0.0
     */
    virtual void write(const LogMessage& msg) = 0;
    
    /**
     * @brief 刷新输出缓冲区
     * @note 确保所有待输出的内容都被实际输出
     * @since 1.0.0
     */
    virtual void flush() = 0;
    
    /**
     * @brief 关闭输出
     * @note 释放相关资源，关闭后不应再调用write或flush
     * @since 1.0.0
     */
    virtual void close() = 0;
    
    /**
     * @brief 检查输出是否可用
     * @return true表示输出可用，false表示输出不可用
     * @since 1.0.0
     */
    virtual bool isAvailable() const = 0;
};

/**
 * @brief 文件输出实现
 * @details 将日志输出到文件，支持文件轮转和大小限制
 * @note 此实现是线程安全的
 * @since 1.0.0
 */
class FileOutput : public ILogOutput {
private:
    std::string filePath_;              ///< 文件路径
    std::ofstream fileStream_;          ///< 文件流
    mutable std::mutex fileMutex_;      ///< 文件操作互斥锁
    size_t currentFileSize_;            ///< 当前文件大小
    size_t maxFileSize_;                ///< 最大文件大小
    int maxFileCount_;                  ///< 最大文件数量
    bool isOpen_;                       ///< 文件是否打开
    
public:
    /**
     * @brief 构造函数
     * @param[in] path 文件路径
     * @param[in] maxSize 最大文件大小（字节）
     * @param[in] maxCount 最大文件数量
     * @since 1.0.0
     */
    explicit FileOutput(const std::string& path, 
                       size_t maxSize = 10 * 1024 * 1024,
                       int maxCount = 5);
    
    /**
     * @brief 析构函数
     * @since 1.0.0
     */
    ~FileOutput() override;
    
    // 禁用拷贝构造和赋值
    FileOutput(const FileOutput&) = delete;
    FileOutput& operator=(const FileOutput&) = delete;
    
    // 允许移动构造和赋值
    FileOutput(FileOutput&&) noexcept;
    FileOutput& operator=(FileOutput&&) noexcept;
    
    void write(const LogMessage& msg) override;
    void flush() override;
    void close() override;
    bool isAvailable() const override;
    
    /**
     * @brief 设置文件路径
     * @param[in] path 新的文件路径
     * @since 1.0.0
     */
    void setFilePath(const std::string& path);
    
    /**
     * @brief 获取当前文件路径
     * @return 当前文件路径
     * @since 1.0.0
     */
    std::string getFilePath() const;
    
private:
    /**
     * @brief 打开文件
     * @return true表示成功，false表示失败
     * @since 1.0.0
     */
    bool openFile();
    
    /**
     * @brief 轮转文件
     * @since 1.0.0
     */
    void rotateFile();
    
    /**
     * @brief 格式化日志消息
     * @param[in] msg 日志消息
     * @return 格式化后的字符串
     * @since 1.0.0
     */
    std::string formatMessage(const LogMessage& msg);
};

/**
 * @brief 控制台输出实现
 * @details 将日志输出到控制台，支持颜色输出
 * @note 此实现是线程安全的
 * @since 1.0.0
 */
class ConsoleOutput : public ILogOutput {
private:
    bool enableColor_;                  ///< 是否启用颜色
    mutable std::mutex consoleMutex_;   ///< 控制台输出互斥锁
    
public:
    /**
     * @brief 构造函数
     * @param[in] enableColor 是否启用颜色输出
     * @since 1.0.0
     */
    explicit ConsoleOutput(bool enableColor = true);
    
    void write(const LogMessage& msg) override;
    void flush() override;
    void close() override;
    bool isAvailable() const override;
    
    /**
     * @brief 设置颜色输出
     * @param[in] enable 是否启用
     * @since 1.0.0
     */
    void setColorEnabled(bool enable);
    
private:
    /**
     * @brief 获取颜色代码
     * @param[in] level 日志级别
     * @return ANSI颜色代码字符串
     * @since 1.0.0
     */
    std::string getColorCode(LogLevel level);
    
    /**
     * @brief 获取重置颜色代码
     * @return ANSI重置颜色代码字符串
     * @since 1.0.0
     */
    std::string getResetCode();
    
    /**
     * @brief 格式化日志消息
     * @param[in] msg 日志消息
     * @return 格式化后的字符串
     * @since 1.0.0
     */
    std::string formatMessage(const LogMessage& msg);
};

/**
 * @brief 网络输出实现
 * @details 将日志通过网络发送到远程服务器
 * @note 此实现是线程安全的
 * @since 1.0.0
 */
class NetworkOutput : public ILogOutput {
private:
    std::string host_;                  ///< 服务器地址
    int port_;                          ///< 服务器端口
    bool isConnected_;                  ///< 连接状态
    mutable std::mutex networkMutex_;   ///< 网络操作互斥锁
    
public:
    /**
     * @brief 构造函数
     * @param[in] host 服务器地址
     * @param[in] port 服务器端口
     * @since 1.0.0
     */
    NetworkOutput(const std::string& host, int port);
    
    void write(const LogMessage& msg) override;
    void flush() override;
    void close() override;
    bool isAvailable() const override;
    
    /**
     * @brief 连接服务器
     * @return true表示成功，false表示失败
     * @since 1.0.0
     */
    bool connect();
    
    /**
     * @brief 断开连接
     * @since 1.0.0
     */
    void disconnect();
    
    /**
     * @brief 检查连接状态
     * @return true表示已连接，false表示未连接
     * @since 1.0.0
     */
    bool isConnected() const;
    
private:
    /**
     * @brief 发送数据
     * @param[in] data 要发送的数据
     * @return true表示成功，false表示失败
     * @since 1.0.0
     */
    bool sendData(const std::string& data);
    
    /**
     * @brief 格式化日志消息
     * @param[in] msg 日志消息
     * @return 格式化后的字符串
     * @since 1.0.0
     */
    std::string formatMessage(const LogMessage& msg);
};

} // namespace async_log
