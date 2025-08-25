/**
 * @file logFactory.hpp
 * @brief 日志工厂类
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 实现工厂模式，根据配置创建和管理日志输出对象，支持插件式扩展
 * @note 此实现是线程安全的
 * @see ILogOutput, LogDecorator, LogConfig
 * @since 1.0.0
 */

#pragma once

#include "logOutput.hpp"
#include "logDecorator.hpp"
#include "logTypes.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>

namespace async_log {

/**
 * @brief 日志输出工厂类
 * @details 负责根据配置创建各种类型的日志输出对象，支持装饰器组合
 * @note 此实现是线程安全的
 * @since 1.0.0
 */
class LogOutputFactory {
public:
    /**
     * @brief 输出类型枚举
     * @since 1.0.0
     */
    enum class OutputType {
        FILE,       ///< 文件输出
        CONSOLE,    ///< 控制台输出
        NETWORK,    ///< 网络输出
        CUSTOM      ///< 自定义输出
    };
    
    /**
     * @brief 装饰器类型枚举
     * @since 1.0.0
     */
    enum class DecoratorType {
        TIMESTAMP,      ///< 时间戳装饰器
        COLOR,          ///< 颜色装饰器
        COMPRESSION,    ///< 压缩装饰器
        FILTER,         ///< 过滤装饰器
        FORMAT,         ///< 格式化装饰器
        CUSTOM          ///< 自定义装饰器
    };
    
    /**
     * @brief 输出创建函数类型
     * @since 1.0.0
     */
    using OutputCreator = std::function<std::unique_ptr<ILogOutput>(const LogConfig&)>;
    
    /**
     * @brief 装饰器创建函数类型
     * @since 1.0.0
     */
    using DecoratorCreator = std::function<std::unique_ptr<LogDecorator>(std::unique_ptr<ILogOutput>, const LogConfig&)>;
    
private:
    // 内置输出类型创建器
    static std::unordered_map<std::string, OutputCreator> outputCreators_;
    
    // 内置装饰器类型创建器
    static std::unordered_map<std::string, DecoratorCreator> decoratorCreators_;
    
    // 线程安全
    static std::mutex factoryMutex_;
    
public:
    /**
     * @brief 创建日志输出对象
     * @param[in] type 输出类型字符串
     * @param[in] config 配置对象
     * @return 指向输出对象的智能指针，失败时返回nullptr
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static std::unique_ptr<ILogOutput> createOutput(const std::string& type, 
                                                   const LogConfig& config);
    
    /**
     * @brief 创建日志输出对象（枚举版本）
     * @param[in] type 输出类型枚举
     * @param[in] config 配置对象
     * @return 指向输出对象的智能指针，失败时返回nullptr
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static std::unique_ptr<ILogOutput> createOutput(OutputType type, 
                                                   const LogConfig& config);
    
    /**
     * @brief 创建装饰器
     * @param[in] type 装饰器类型字符串
     * @param[in] output 要装饰的输出对象
     * @param[in] config 配置对象
     * @return 指向装饰器的智能指针，失败时返回nullptr
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static std::unique_ptr<LogDecorator> createDecorator(const std::string& type,
                                                        std::unique_ptr<ILogOutput> output,
                                                        const LogConfig& config);
    
    /**
     * @brief 创建装饰器（枚举版本）
     * @param[in] type 装饰器类型枚举
     * @param[in] output 要装饰的输出对象
     * @param[in] config 配置对象
     * @return 指向装饰器的智能指针，失败时返回nullptr
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static std::unique_ptr<LogDecorator> createDecorator(DecoratorType type,
                                                        std::unique_ptr<ILogOutput> output,
                                                        const LogConfig& config);
    
    /**
     * @brief 注册自定义输出类型
     * @param[in] type 输出类型名称
     * @param[in] creator 创建函数
     * @return true表示成功，false表示失败
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static bool registerOutputType(const std::string& type, OutputCreator creator);
    
    /**
     * @brief 注册自定义装饰器类型
     * @param[in] type 装饰器类型名称
     * @param[in] creator 创建函数
     * @return true表示成功，false表示失败
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static bool registerDecoratorType(const std::string& type, DecoratorCreator creator);
    
    /**
     * @brief 取消注册输出类型
     * @param[in] type 输出类型名称
     * @return true表示成功，false表示失败
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static bool unregisterOutputType(const std::string& type);
    
    /**
     * @brief 取消注册装饰器类型
     * @param[in] type 装饰器类型名称
     * @return true表示成功，false表示失败
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static bool unregisterDecoratorType(const std::string& type);
    
    /**
     * @brief 检查输出类型是否已注册
     * @param[in] type 输出类型名称
     * @return true表示已注册，false表示未注册
     * @since 1.0.0
     */
    static bool isOutputTypeRegistered(const std::string& type);
    
    /**
     * @brief 检查装饰器类型是否已注册
     * @param[in] type 装饰器类型名称
     * @return true表示已注册，false表示未注册
     * @since 1.0.0
     */
    static bool isDecoratorTypeRegistered(const std::string& type);
    
    /**
     * @brief 获取已注册的输出类型列表
     * @return 已注册输出类型的名称列表
     * @since 1.0.0
     */
    static std::vector<std::string> getRegisteredOutputTypes();
    
    /**
     * @brief 获取已注册的装饰器类型列表
     * @return 已注册装饰器类型的名称列表
     * @since 1.0.0
     */
    static std::vector<std::string> getRegisteredDecoratorTypes();
    
    /**
     * @brief 创建带装饰器的输出对象
     * @param[in] outputType 输出类型
     * @param[in] decoratorTypes 装饰器类型列表
     * @param[in] config 配置对象
     * @return 指向最终输出对象的智能指针，失败时返回nullptr
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static std::unique_ptr<ILogOutput> createDecoratedOutput(
        const std::string& outputType,
        const std::vector<std::string>& decoratorTypes,
        const LogConfig& config);
    
    /**
     * @brief 从配置文件创建输出对象
     * @param[in] config 配置对象
     * @return 指向输出对象的智能指针，失败时返回nullptr
     * @note 此操作是线程安全的
     * @since 1.0.0
     */
    static std::unique_ptr<ILogOutput> createFromConfig(const LogConfig& config);
    
    /**
     * @brief 初始化内置类型
     * @note 此函数在类首次使用时自动调用
     * @since 1.0.0
     */
    static void initializeBuiltinTypes();
    
private:
    // 内置输出类型创建函数
    static std::unique_ptr<ILogOutput> createFileOutput(const LogConfig& config);
    static std::unique_ptr<ILogOutput> createConsoleOutput(const LogConfig& config);
    static std::unique_ptr<ILogOutput> createNetworkOutput(const LogConfig& config);
    
    // 内置装饰器创建函数
    static std::unique_ptr<LogDecorator> createTimestampDecorator(
        std::unique_ptr<ILogOutput> output, const LogConfig& config);
    static std::unique_ptr<LogDecorator> createColorDecorator(
        std::unique_ptr<ILogOutput> output, const LogConfig& config);
    static std::unique_ptr<LogDecorator> createCompressionDecorator(
        std::unique_ptr<ILogOutput> output, const LogConfig& config);
    static std::unique_ptr<LogDecorator> createFilterDecorator(
        std::unique_ptr<ILogOutput> output, const LogConfig& config);
    static std::unique_ptr<LogDecorator> createFormatDecorator(
        std::unique_ptr<ILogOutput> output, const LogConfig& config);
    
    // 字符串转换
    static std::string outputTypeToString(OutputType type);
    static OutputType stringToOutputType(const std::string& str);
    static std::string decoratorTypeToString(DecoratorType type);
    static DecoratorType stringToDecoratorType(const std::string& str);
};

} // namespace async_log
