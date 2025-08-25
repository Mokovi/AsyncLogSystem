/**
 * @file logDecorator.hpp
 * @brief 日志装饰器类
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现装饰器模式，为日志输出添加额外功能，如时间戳、颜色、压缩等
 * @note 装饰器可以组合使用，形成功能链
 * @see ILogOutput, LogManager
 * @since 1.0.0
 */

#pragma once

#include "logOutput.hpp"
#include "logTypes.hpp"
#include <memory>
#include <string>
#include <regex>

namespace async_log {

/**
 * @brief 基础装饰器类
 * @details 装饰器模式的基类，包装ILogOutput接口
 * @note 此实现是线程安全的
 * @since 1.0.0
 */
class LogDecorator : public ILogOutput {
protected:
    std::unique_ptr<ILogOutput> wrapped_;  ///< 被装饰的输出对象
    
public:
    /**
     * @brief 构造函数
     * @param[in] output 要装饰的输出对象
     * @since 1.0.0
     */
    explicit LogDecorator(std::unique_ptr<ILogOutput> output);
    
    /**
     * @brief 析构函数
     * @since 1.0.0
     */
    ~LogDecorator() override = default;
    
    // 基础接口实现
    void write(const LogMessage& msg) override;
    void flush() override;
    void close() override;
    bool isAvailable() const override;
    
    /**
     * @brief 获取被装饰的输出对象
     * @return 指向被装饰输出对象的指针
     * @since 1.0.0
     */
    ILogOutput* getWrappedOutput() const;
    
    /**
     * @brief 替换被装饰的输出对象
     * @param[in] output 新的输出对象
     * @since 1.0.0
     */
    void setWrappedOutput(std::unique_ptr<ILogOutput> output);
};

/**
 * @brief 时间戳装饰器
 * @details 为日志消息添加时间戳信息
 * @since 1.0.0
 */
class TimestampDecorator : public LogDecorator {
private:
    std::string format_;  ///< 时间格式字符串
    
public:
    /**
     * @brief 构造函数
     * @param[in] output 要装饰的输出对象
     * @param[in] timeFormat 时间格式，默认为"%Y-%m-%d %H:%M:%S"
     * @since 1.0.0
     */
    TimestampDecorator(std::unique_ptr<ILogOutput> output, 
                      const std::string& timeFormat = "%Y-%m-%d %H:%M:%S");
    
    void write(const LogMessage& msg) override;
    
    /**
     * @brief 设置时间格式
     * @param[in] format 新的时间格式
     * @since 1.0.0
     */
    void setTimeFormat(const std::string& format);
    
    /**
     * @brief 获取当前时间格式
     * @return 当前时间格式字符串
     * @since 1.0.0
     */
    std::string getTimeFormat() const;
    
private:
    /**
     * @brief 获取当前时间戳
     * @return 格式化的时间戳字符串
     * @since 1.0.0
     */
    std::string getCurrentTimestamp();
    
    /**
     * @brief 格式化时间
     * @param[in] timePoint 时间点
     * @return 格式化的时间字符串
     * @since 1.0.0
     */
    std::string formatTime(const std::chrono::system_clock::time_point& timePoint);
};

/**
 * @brief 颜色装饰器
 * @details 为控制台输出添加颜色信息
 * @since 1.0.0
 */
class ColorDecorator : public LogDecorator {
private:
    bool enableColor_;  ///< 是否启用颜色
    
public:
    /**
     * @brief 构造函数
     * @param[in] output 要装饰的输出对象
     * @param[in] enableColor 是否启用颜色，默认为true
     * @since 1.0.0
     */
    explicit ColorDecorator(std::unique_ptr<ILogOutput> output, bool enableColor = true);
    
    void write(const LogMessage& msg) override;
    
    /**
     * @brief 设置颜色启用状态
     * @param[in] enable 是否启用颜色
     * @since 1.0.0
     */
    void setColorEnabled(bool enable);
    
    /**
     * @brief 检查颜色是否启用
     * @return true表示启用，false表示禁用
     * @since 1.0.0
     */
    bool isColorEnabled() const;
    
private:
    /**
     * @brief 获取颜色代码
     * @param[in] level 日志级别
     * @return ANSI颜色代码字符串
     * @since 1.0.0
     */
    std::string getColorCode(LogLevel level);
    
    /**
     * @brief 重置颜色代码
     * @return ANSI重置颜色代码字符串
     * @since 1.0.0
     */
    std::string getResetCode();
};

/**
 * @brief 压缩装饰器
 * @details 对日志消息进行压缩处理
 * @since 1.0.0
 */
class CompressionDecorator : public LogDecorator {
private:
    bool enableCompression_;  ///< 是否启用压缩
    size_t minSize_;          ///< 最小压缩大小
    
public:
    /**
     * @brief 构造函数
     * @param[in] output 要装饰的输出对象
     * @param[in] enableCompression 是否启用压缩，默认为true
     * @param[in] minSize 最小压缩大小，默认为1024字节
     * @since 1.0.0
     */
    CompressionDecorator(std::unique_ptr<ILogOutput> output, 
                        bool enableCompression = true, 
                        size_t minSize = 1024);
    
    void write(const LogMessage& msg) override;
    
    /**
     * @brief 设置压缩启用状态
     * @param[in] enable 是否启用压缩
     * @since 1.0.0
     */
    void setCompressionEnabled(bool enable);
    
    /**
     * @brief 设置最小压缩大小
     * @param[in] minSize 最小压缩大小（字节）
     * @since 1.0.0
     */
    void setMinCompressionSize(size_t minSize);
    
private:
    /**
     * @brief 压缩数据
     * @param[in] data 要压缩的数据
     * @return 压缩后的数据
     * @since 1.0.0
     */
    std::string compress(const std::string& data);
    
    /**
     * @brief 解压数据
     * @param[in] data 要解压的数据
     * @return 解压后的数据
     * @since 1.0.0
     */
    std::string decompress(const std::string& data);
};

/**
 * @brief 过滤装饰器
 * @details 根据条件过滤日志消息
 * @since 1.0.0
 */
class FilterDecorator : public LogDecorator {
private:
    std::function<bool(const LogMessage&)> filter_;  ///< 过滤函数
    
public:
    /**
     * @brief 构造函数
     * @param[in] output 要装饰的输出对象
     * @param[in] filter 过滤函数，返回true表示通过，false表示过滤
     * @since 1.0.0
     */
    FilterDecorator(std::unique_ptr<ILogOutput> output, 
                   std::function<bool(const LogMessage&)> filter);
    
    void write(const LogMessage& msg) override;
    
    /**
     * @brief 设置过滤函数
     * @param[in] filter 新的过滤函数
     * @since 1.0.0
     */
    void setFilter(std::function<bool(const LogMessage&)> filter);
    
    /**
     * @brief 清除过滤函数
     * @since 1.0.0
     */
    void clearFilter();
    
private:
    /**
     * @brief 检查消息是否应该通过
     * @param[in] msg 要检查的消息
     * @return true表示应该通过，false表示应该过滤
     * @since 1.0.0
     */
    bool shouldPass(const LogMessage& msg);
};

/**
 * @brief 格式化装饰器
 * @details 自定义日志消息的格式
 * @since 1.0.0
 */
class FormatDecorator : public LogDecorator {
private:
    std::string format_;  ///< 格式字符串
    
public:
    /**
     * @brief 构造函数
     * @param[in] output 要装饰的输出对象
     * @param[in] format 格式字符串
     * @since 1.0.0
     */
    FormatDecorator(std::unique_ptr<ILogOutput> output, const std::string& format);
    
    void write(const LogMessage& msg) override;
    
    /**
     * @brief 设置格式字符串
     * @param[in] format 新的格式字符串
     * @since 1.0.0
     */
    void setFormat(const std::string& format);
    
    /**
     * @brief 获取当前格式字符串
     * @return 当前格式字符串
     * @since 1.0.0
     */
    std::string getFormat() const;
    
private:
    /**
     * @brief 格式化消息
     * @param[in] msg 原始消息
     * @return 格式化后的消息
     * @since 1.0.0
     */
    std::string formatMessage(const LogMessage& msg);
    
    /**
     * @brief 替换格式占位符
     * @param[in] format 格式字符串
     * @param[in] msg 日志消息
     * @return 替换后的字符串
     * @since 1.0.0
     */
    std::string replacePlaceholders(const std::string& format, const LogMessage& msg);
};

} // namespace async_log
