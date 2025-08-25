/**
 * @file advancedUsage.cpp
 * @brief AsyncLogSystem 高级使用示例
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 演示AsyncLogSystem的高级功能和自定义扩展
 * @see LogManager, ILogOutput, LogDecorator, LogOutputFactory
 * @since 1.0.0
 */

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>
#include <functional>
#include <random>
#include <iomanip>
#include <sstream>

// 包含日志系统头文件
#include "logManager.hpp"
#include "logOutput.hpp"
#include "logDecorator.hpp"
#include "logFactory.hpp"

using namespace async_log;

/**
 * @brief 自定义日志输出类
 * @details 演示如何创建自定义的日志输出
 */
class CustomOutput : public ILogOutput {
private:
    std::string name_;
    std::vector<std::string> buffer_;
    mutable std::mutex mutex_;
    size_t maxBufferSize_;
    
public:
    explicit CustomOutput(const std::string& name, size_t maxBufferSize = 1000)
        : name_(name), maxBufferSize_(maxBufferSize) {}
    
    void write(const LogMessage& msg) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // 格式化日志消息
        std::string formatted = formatMessage(msg);
        buffer_.push_back(formatted);
        
        // 限制缓冲区大小
        if (buffer_.size() > maxBufferSize_) {
            buffer_.erase(buffer_.begin());
        }
        
        // 模拟输出延迟
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    void flush() override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[" << name_ << "] 刷新缓冲区，共 " << buffer_.size() << " 条日志" << std::endl;
    }
    
    void close() override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[" << name_ << "] 关闭输出，缓冲区剩余 " << buffer_.size() << " 条日志" << std::endl;
        buffer_.clear();
    }
    
    bool isAvailable() const override {
        return true;
    }
    
    // 获取缓冲区内容
    std::vector<std::string> getBuffer() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return buffer_;
    }
    
private:
    std::string formatMessage(const LogMessage& msg) {
        std::stringstream ss;
        ss << "[" << name_ << "] "
           << levelToString(msg.level) << " "
           << msg.message;
        return ss.str();
    }
};

/**
 * @brief 自定义装饰器类
 * @details 演示如何创建自定义的日志装饰器
 */
class CustomDecorator : public LogDecorator {
private:
    std::string prefix_;
    std::function<std::string(const LogMessage&)> formatter_;
    
public:
    CustomDecorator(std::unique_ptr<ILogOutput> output, 
                   const std::string& prefix,
                   std::function<std::string(const LogMessage&)> formatter = nullptr)
        : LogDecorator(std::move(output)), prefix_(prefix), formatter_(formatter) {}
    
    void write(const LogMessage& msg) override {
        if (formatter_) {
            // 使用自定义格式化函数
            LogMessage enhancedMsg = msg;
            enhancedMsg.message = prefix_ + formatter_(msg);
            wrapped_->write(enhancedMsg);
        } else {
            // 使用默认格式化
            LogMessage enhancedMsg = msg;
            enhancedMsg.message = prefix_ + msg.message;
            wrapped_->write(enhancedMsg);
        }
    }
};

/**
 * @brief 自定义日志级别示例
 * @details 演示如何使用自定义日志级别
 */
void customLogLevelExample() {
    std::cout << "\n=== 自定义日志级别示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    // 创建自定义输出
    auto customOutput = std::make_unique<CustomOutput>("CustomLevel");
    logManager.addOutput(std::move(customOutput));
    
    // 记录不同级别的日志
    for (int i = 0; i < 5; ++i) {
        LogLevel level = static_cast<LogLevel>(i);
        logManager.log(level, "自定义级别测试 " + std::to_string(i));
    }
    
    std::cout << "自定义日志级别示例完成" << std::endl;
}

/**
 * @brief 自定义装饰器示例
 * @details 演示如何使用自定义装饰器
 */
void customDecoratorExample() {
    std::cout << "\n=== 自定义装饰器示例 ===" << std::endl;
    
    // 创建基础输出
    auto baseOutput = std::make_unique<ConsoleOutput>();
    
    // 创建自定义装饰器
    auto customDecorator = std::make_unique<CustomDecorator>(
        std::move(baseOutput),
        "[CUSTOM] ",
        [](const LogMessage& msg) -> std::string {
            return "[" + levelToString(msg.level) + "] " + msg.message + " (自定义格式化)";
        }
    );
    
    // 使用装饰器输出日志
    customDecorator->write(LogMessage(LogLevel::INFO, "这是通过自定义装饰器输出的日志"));
    customDecorator->flush();
    
    std::cout << "自定义装饰器示例完成" << std::endl;
}

/**
 * @brief 批量日志处理示例
 * @details 演示如何批量处理日志消息
 */
void batchProcessingExample() {
    std::cout << "\n=== 批量日志处理示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    // 批量记录日志
    std::vector<std::string> messages;
    for (int i = 0; i < 100; ++i) {
        messages.push_back("批量消息 " + std::to_string(i));
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 并行记录日志
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&logManager, &messages, i]() {
            for (int j = i * 10; j < (i + 1) * 10; ++j) {
                logManager.log(LogLevel::INFO, messages[j]);
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "批量处理完成，耗时 " << duration.count() << " 毫秒" << std::endl;
}

/**
 * @brief 日志过滤示例
 * @details 演示如何使用过滤装饰器
 */
void filteringExample() {
    std::cout << "\n=== 日志过滤示例 ===" << std::endl;
    
    // 创建基础输出
    auto baseOutput = std::make_unique<ConsoleOutput>();
    
    // 创建过滤装饰器，只输出INFO级别以上的日志
    auto filterDecorator = std::make_unique<FilterDecorator>(
        std::move(baseOutput),
        [](const LogMessage& msg) -> bool {
            return static_cast<int>(msg.level) >= static_cast<int>(LogLevel::INFO);
        }
    );
    
    // 测试过滤效果
    filterDecorator->write(LogMessage(LogLevel::DEBUG, "这条DEBUG日志应该被过滤"));
    filterDecorator->write(LogMessage(LogLevel::INFO, "这条INFO日志应该被输出"));
    filterDecorator->write(LogMessage(LogLevel::WARN, "这条WARN日志应该被输出"));
    
    filterDecorator->flush();
    
    std::cout << "日志过滤示例完成" << std::endl;
}

/**
 * @brief 性能监控示例
 * @details 演示如何监控日志系统的性能
 */
void performanceMonitoringExample() {
    std::cout << "\n=== 性能监控示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    // 性能测试参数
    const std::vector<int> testSizes = {1000, 5000, 10000};
    
    for (int size : testSizes) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // 记录指定数量的日志
        for (int i = 0; i < size; ++i) {
            logManager.log(LogLevel::INFO, "性能测试日志 " + std::to_string(i));
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        double throughput = size / (duration.count() / 1000000.0);
        double avgLatency = duration.count() / (double)size;
        
        std::cout << "日志数量: " << size 
                  << ", 吞吐量: " << std::fixed << std::setprecision(2) << throughput << " 条/秒"
                  << ", 平均延迟: " << avgLatency << " 微秒" << std::endl;
    }
    
    std::cout << "性能监控示例完成" << std::endl;
}

/**
 * @brief 错误恢复示例
 * @details 演示日志系统的错误恢复能力
 */
void errorRecoveryExample() {
    std::cout << "\n=== 错误恢复示例 ===" << std::endl;
    
    auto& logManager = LogManager::getInstance();
    
    try {
        // 模拟网络输出失败
        auto networkOutput = std::make_unique<NetworkOutput>("invalid.host", 9999);
        logManager.addOutput(std::move(networkOutput));
        
        // 继续记录日志，系统应该自动切换到其他可用输出
        for (int i = 0; i < 10; ++i) {
            logManager.log(LogLevel::INFO, "错误恢复测试日志 " + std::to_string(i));
        }
        
    } catch (const std::exception& e) {
        std::cout << "捕获到预期异常: " << e.what() << std::endl;
    }
    
    // 验证日志系统仍然可用
    logManager.log(LogLevel::INFO, "错误恢复测试完成");
    
    std::cout << "错误恢复示例完成" << std::endl;
}

/**
 * @brief 主函数
 * @return 程序退出码
 */
int main() {
    std::cout << "AsyncLogSystem 高级使用示例程序" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // 启动日志系统
        auto& logManager = LogManager::getInstance();
        logManager.start();
        
        std::cout << "日志系统启动成功\n" << std::endl;
        
        // 运行各种高级示例
        customLogLevelExample();
        customDecoratorExample();
        batchProcessingExample();
        filteringExample();
        performanceMonitoringExample();
        errorRecoveryExample();
        
        // 等待日志处理完成
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        // 停止日志系统
        logManager.stop();
        
        std::cout << "\n所有高级示例执行完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "程序执行出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
