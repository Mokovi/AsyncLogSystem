/**
 * @file decoratorExample.cpp
 * @brief 装饰器模式详细示例
 * @author Gamma
 * @date 2025-08-25 11:25:00
 * @version 1.0.0
 * @details 详细演示装饰器模式的工作原理和使用方法
 * @see LogDecorator, ILogOutput
 * @since 1.0.0
 */

#include <iostream>
#include <memory>
#include <string>

// 包含日志系统头文件
#include "logOutput.hpp"
#include "logDecorator.hpp"

using namespace async_log;

/**
 * @brief 演示装饰器的基本工作原理
 */
void basicDecoratorExample() {
    std::cout << "\n=== 装饰器基本工作原理演示 ===" << std::endl;
    
    // 1. 创建基础输出（控制台输出）
    auto consoleOutput = std::make_unique<ConsoleOutput>();
    std::cout << "创建基础控制台输出" << std::endl;
    
    // 2. 用时间戳装饰器包装控制台输出
    auto timestampDecorator = std::make_unique<TimestampDecorator>(std::move(consoleOutput));
    std::cout << "用时间戳装饰器包装控制台输出" << std::endl;
    
    // 3. 用颜色装饰器包装时间戳装饰器
    auto colorDecorator = std::make_unique<ColorDecorator>(std::move(timestampDecorator));
    std::cout << "用颜色装饰器包装时间戳装饰器" << std::endl;
    
    // 4. 现在colorDecorator是一个完整的装饰器链
    // 当调用write时，消息会经过：颜色装饰器 -> 时间戳装饰器 -> 控制台输出
    
    // 测试装饰器链
    LogMessage msg(LogLevel::INFO, "这是一条测试消息");
    std::cout << "\n原始消息: " << msg.message << std::endl;
    
    std::cout << "\n通过装饰器链输出:" << std::endl;
    colorDecorator->write(msg);
    
    std::cout << "\n装饰器链的工作原理:" << std::endl;
    std::cout << "1. 颜色装饰器: 添加颜色代码" << std::endl;
    std::cout << "2. 时间戳装饰器: 添加时间戳" << std::endl;
    std::cout << "3. 控制台输出: 实际输出到控制台" << std::endl;
}

/**
 * @brief 演示装饰器的组合使用
 */
void decoratorCombinationExample() {
    std::cout << "\n=== 装饰器组合使用演示 ===" << std::endl;
    
    // 创建不同的装饰器组合
    std::cout << "组合1: 控制台输出 + 时间戳装饰器" << std::endl;
    auto combo1 = std::make_unique<TimestampDecorator>(
        std::make_unique<ConsoleOutput>()
    );
    
    std::cout << "组合2: 控制台输出 + 颜色装饰器" << std::endl;
    auto combo2 = std::make_unique<ColorDecorator>(
        std::make_unique<ConsoleOutput>()
    );
    
    std::cout << "组合3: 控制台输出 + 时间戳装饰器 + 颜色装饰器" << std::endl;
    auto combo3 = std::make_unique<ColorDecorator>(
        std::make_unique<TimestampDecorator>(
            std::make_unique<ConsoleOutput>()
        )
    );
    
    // 测试不同的组合
    LogMessage msg(LogLevel::WARN, "测试不同装饰器组合的效果");
    
    std::cout << "\n组合1效果 (时间戳):" << std::endl;
    combo1->write(msg);
    
    std::cout << "\n组合2效果 (颜色):" << std::endl;
    combo2->write(msg);
    
    std::cout << "\n组合3效果 (时间戳 + 颜色):" << std::endl;
    combo3->write(msg);
}

/**
 * @brief 演示装饰器的动态组合
 */
void dynamicDecoratorExample() {
    std::cout << "\n=== 装饰器动态组合演示 ===" << std::endl;
    
    // 根据配置动态创建装饰器组合
    bool enableTimestamp = true;
    bool enableColor = true;
    bool enableCompression = false;
    
    std::cout << "配置: 时间戳=" << (enableTimestamp ? "开启" : "关闭")
              << ", 颜色=" << (enableColor ? "开启" : "关闭")
              << ", 压缩=" << (enableCompression ? "开启" : "关闭") << std::endl;
    
    // 从最内层开始构建装饰器链
    std::unique_ptr<ILogOutput> currentOutput = std::make_unique<ConsoleOutput>();
    
    if (enableCompression) {
        currentOutput = std::make_unique<CompressionDecorator>(std::move(currentOutput));
        std::cout << "添加压缩装饰器" << std::endl;
    }
    
    if (enableTimestamp) {
        currentOutput = std::make_unique<TimestampDecorator>(std::move(currentOutput));
        std::cout << "添加时间戳装饰器" << std::endl;
    }
    
    if (enableColor) {
        currentOutput = std::make_unique<ColorDecorator>(std::move(currentOutput));
        std::cout << "添加颜色装饰器" << std::endl;
    }
    
    // 测试动态组合的装饰器
    LogMessage msg(LogLevel::INFO, "动态组合装饰器的测试消息");
    std::cout << "\n最终装饰器链输出:" << std::endl;
    currentOutput->write(msg);
}

/**
 * @brief 演示装饰器与普通输出的区别
 */
void decoratorVsOutputExample() {
    std::cout << "\n=== 装饰器与普通输出的区别演示 ===" << std::endl;
    
    // 普通输出：直接输出到目标
    std::cout << "普通输出 (ConsoleOutput):" << std::endl;
    auto plainOutput = std::make_unique<ConsoleOutput>();
    plainOutput->write(LogMessage(LogLevel::INFO, "普通消息"));
    
    // 装饰器输出：先装饰，再输出到目标
    std::cout << "\n装饰器输出 (TimestampDecorator + ConsoleOutput):" << std::endl;
    auto decoratedOutput = std::make_unique<TimestampDecorator>(
        std::make_unique<ConsoleOutput>()
    );
    decoratedOutput->write(LogMessage(LogLevel::INFO, "装饰后的消息"));
    
    std::cout << "\n关键区别:" << std::endl;
    std::cout << "1. 普通输出: 直接输出原始消息" << std::endl;
    std::cout << "2. 装饰器输出: 先修改消息(添加时间戳)，再输出修改后的消息" << std::endl;
    std::cout << "3. 装饰器本身不输出，它只是修改消息，然后委托给被包装的输出" << std::endl;
}

/**
 * @brief 主函数
 * @return 程序退出码
 */
int main() {
    std::cout << "AsyncLogSystem 装饰器模式详细示例程序" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        // 运行各种装饰器示例
        basicDecoratorExample();
        decoratorCombinationExample();
        dynamicDecoratorExample();
        decoratorVsOutputExample();
        
        std::cout << "\n所有装饰器示例执行完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "程序执行出错: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
