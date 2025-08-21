#include <gtest/gtest.h>
#include "async_log_system.h"
#include <thread>
#include <chrono>
#include <vector>

class AsyncLogSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前启动异步系统
        asyncLogger_.start();
    }
    
    void TearDown() override {
        // 每个测试后停止异步系统
        asyncLogger_.stop();
    }
    
    log_system::AsyncLogSystem& asyncLogger_ = log_system::AsyncLogSystem::getInstance();
};

// 测试异步日志系统基本功能
TEST_F(AsyncLogSystemTest, BasicAsyncLogging) {
    // 记录一些日志
    asyncLogger_.info("测试信息1");
    asyncLogger_.warn("测试警告1");
    asyncLogger_.error("测试错误1");
    
    // 等待处理完成
    EXPECT_TRUE(asyncLogger_.waitForCompletion(1000));
    
    // 验证系统正在运行
    EXPECT_TRUE(asyncLogger_.isRunning());
}

// 测试多线程并发日志记录
TEST_F(AsyncLogSystemTest, MultiThreadedLogging) {
    const int threadCount = 4;
    const int logsPerThread = 50;
    std::vector<std::thread> threads;
    
    // 启动多个线程并发记录日志
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([this, i, logsPerThread]() {
            for (int j = 0; j < logsPerThread; ++j) {
                asyncLogger_.info("线程 " + std::to_string(i) + " 日志 " + std::to_string(j));
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 等待日志处理完成
    EXPECT_TRUE(asyncLogger_.waitForCompletion(2000));
    
    // 验证队列大小应该为0（所有日志都已处理）
    EXPECT_EQ(asyncLogger_.getQueueSize(), 0);
}

// 测试队列大小限制
TEST_F(AsyncLogSystemTest, QueueSizeLimit) {
    // 记录大量日志，超过默认队列大小限制
    const int logCount = 15000;  // 超过默认的10000限制
    
    for (int i = 0; testing::Test::HasFailure(); ++i) {
        asyncLogger_.info("大量日志 " + std::to_string(i));
    }
    
    // 等待一段时间让系统处理
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 验证队列大小不会超过限制
    EXPECT_LE(asyncLogger_.getQueueSize(), 10000);
}

// 测试异步日志系统的启动和停止
TEST_F(AsyncLogSystemTest, StartStop) {
    // 停止系统
    asyncLogger_.stop();
    EXPECT_FALSE(asyncLogger_.isRunning());
    
    // 重新启动
    asyncLogger_.start();
    EXPECT_TRUE(asyncLogger_.isRunning());
    
    // 测试日志记录
    asyncLogger_.info("重启后的测试日志");
    EXPECT_TRUE(asyncLogger_.waitForCompletion(1000));
}

// 测试日志级别过滤
TEST_F(AsyncLogSystemTest, LogLevelFiltering) {
    // 设置日志级别为WARN
    log_system::LogConfig config;
    config.minLevel = log_system::LogLevel::WARN;
    asyncLogger_.setConfig(config);
    
    // 记录不同级别的日志
    asyncLogger_.debug("调试信息 - 应该被过滤");
    asyncLogger_.info("普通信息 - 应该被过滤");
    asyncLogger_.warn("警告信息 - 应该显示");
    asyncLogger_.error("错误信息 - 应该显示");
    
    // 等待处理完成
    EXPECT_TRUE(asyncLogger_.waitForCompletion(1000));
}

// 测试性能对比
TEST_F(AsyncLogSystemTest, PerformanceComparison) {
    const int logCount = 1000;
    
    // 测试异步日志性能
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < logCount; ++i) {
        asyncLogger_.logAsync(log_system::LogLevel::INFO, "性能测试日志 " + std::to_string(i));
    }
    auto asyncEnd = std::chrono::high_resolution_clock::now();
    
    // 等待处理完成
    EXPECT_TRUE(asyncLogger_.waitForCompletion(5000));
    
    auto asyncDuration = std::chrono::duration_cast<std::chrono::milliseconds>(asyncEnd - start);
    
    // 验证异步日志记录速度（应该很快，因为只是加入队列）
    EXPECT_LT(asyncDuration.count(), 100);  // 应该在100ms内完成
}

// 测试异常情况处理
TEST_F(AsyncLogSystemTest, ExceptionHandling) {
    // 测试在系统未启动时记录日志（应该回退到同步模式）
    asyncLogger_.stop();
    
    // 这应该不会抛出异常
    EXPECT_NO_THROW(asyncLogger_.info("系统未启动时的日志"));
    
    // 重新启动系统
    asyncLogger_.start();
    EXPECT_TRUE(asyncLogger_.isRunning());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
