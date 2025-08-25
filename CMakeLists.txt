# =============================================================================
# AsyncLogSystem 项目构建配置文件
# =============================================================================
# 
# 项目信息:
# - 项目名称: AsyncLogSystem
# - 版本: 1.0.0
# - 语言: C++
# - 最低CMake版本: 3.15
# - 作者: Gamma
# - 创建时间: 2025-08-25 11:25:00
#
# 功能特性:
# - 高性能异步日志系统
# - 支持多种输出方式（文件、控制台、网络）
# - 装饰器模式支持
# - 工厂模式组件创建
# - 无锁队列消息传递
# - 多线程并发支持
#
# 构建目标:
# - async_log_system: 静态库
# - async_log_demo: 演示程序
# - tests: 测试套件
# - examples: 示例程序
# =============================================================================

# 设置最低CMake版本要求
# 选择3.15版本是因为它支持现代C++特性和CMake最佳实践
cmake_minimum_required(VERSION 3.15)

# 定义项目基本信息
# 项目名称、版本号和使用的编程语言
project(AsyncLogSystem VERSION 1.0.0 LANGUAGES CXX)

# =============================================================================
# C++标准配置
# =============================================================================
# 设置C++17标准，这是现代C++开发的最佳选择
# - 支持智能指针、移动语义、lambda表达式等现代特性
# - 与大多数现代编译器和平台兼容
# - 性能优秀，功能丰富
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# 编译器选项配置
# =============================================================================
# 全局编译选项设置
# -Wall: 启用所有常见警告
# -Wextra: 启用额外警告
# -Werror: 将警告视为错误（可选，用于严格模式）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Debug配置编译选项
# -g: 生成调试信息
# -O0: 禁用优化，便于调试
# -DDEBUG: 定义DEBUG宏，用于条件编译
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# Release配置编译选项
# -O3: 最高级别优化
# -DNDEBUG: 定义NDEBUG宏，禁用assert
# -DNDEBUG通常用于生产环境，提高性能
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# =============================================================================
# 依赖库配置
# =============================================================================
# 查找并配置线程库
# Threads::Threads是跨平台的线程库抽象
# 在Linux上通常是pthread，在Windows上是系统线程
# REQUIRED表示如果找不到线程库，构建将失败
find_package(Threads REQUIRED)

# =============================================================================
# 源文件配置
# =============================================================================
# 将所有源文件列在一个变量中，便于管理和复用
# 注意：文件名已更新为camelCase命名法
set(SOURCES
    src/logTypes.cpp          # 日志类型定义和转换函数
    src/logOutput.cpp         # 日志输出接口实现（文件、控制台、网络）
    src/logManager.cpp        # 日志管理器核心实现
    src/logDispatcher.cpp     # 日志分发器实现
    src/logDecorator.cpp      # 装饰器模式实现
    src/logFactory.cpp        # 工厂模式实现
    src/main.cpp              # 主程序入口和演示代码
)

# =============================================================================
# 头文件配置
# =============================================================================
# 将所有头文件列在一个变量中，便于安装和IDE显示
# 注意：文件名已更新为camelCase命名法
set(HEADERS
    include/logTypes.hpp          # 基础类型定义（日志级别、消息结构、配置）
    include/logOutput.hpp         # 输出接口抽象和具体实现
    include/logManager.hpp        # 日志管理器主类声明
    include/logDispatcher.hpp     # 日志分发器类声明
    include/logDecorator.hpp      # 装饰器基类和具体装饰器
    include/logFactory.hpp        # 工厂类声明
    include/lockFreeQueue.hpp     # 无锁队列模板类
)

# =============================================================================
# 主库构建配置
# =============================================================================
# 创建静态库目标
# 静态库的优势：
# - 链接时直接嵌入可执行文件
# - 运行时不需要额外的库文件
# - 性能更好，启动更快
add_library(async_log_system ${SOURCES} ${HEADERS})

# 设置库的包含目录
# PUBLIC表示：
# - 库内部使用这些头文件
# - 链接此库的其他目标也会继承这些包含路径
# 这样用户就可以通过 #include <async_log_system/...> 使用库
target_include_directories(async_log_system PUBLIC include)

# 链接依赖库
# 链接线程库，这是日志系统多线程功能的基础
target_link_libraries(async_log_system Threads::Threads)

# =============================================================================
# 可执行文件构建配置
# =============================================================================
# 创建演示程序可执行文件
# 这个程序展示日志系统的各种功能和使用方法
add_executable(async_log_demo src/main.cpp)

# 将演示程序与主库链接
# 演示程序需要访问日志系统的所有功能
target_link_libraries(async_log_demo async_log_system)

# =============================================================================
# 测试配置
# =============================================================================
# 启用CTest支持
# 这允许使用add_test()和通过ctest命令运行测试
enable_testing()

# 添加测试子目录
# tests/CMakeLists.txt中定义具体的测试目标
# 通常使用Google Test或Catch2等测试框架
add_subdirectory(tests)

# =============================================================================
# 示例程序配置
# =============================================================================
# 添加示例程序子目录
# examples/CMakeLists.txt中定义示例程序
# 这些示例展示如何在实际项目中使用日志系统
add_subdirectory(examples)

# =============================================================================
# 安装配置
# =============================================================================
# 安装规则配置
# 这些规则用于 'make install' 或 'ninja install' 命令

# 安装库文件
install(TARGETS async_log_system
    LIBRARY DESTINATION lib        # 共享库安装目录（.so/.dylib）
    ARCHIVE DESTINATION lib        # 静态库安装目录（.a）
    RUNTIME DESTINATION bin        # 可执行文件安装目录
)

# 安装头文件
# 将头文件安装到 include/async_log_system/ 目录
# 这样用户就可以通过 #include <async_log_system/...> 使用库
install(FILES ${HEADERS} DESTINATION include/async_log_system)

# =============================================================================
# 输出目录配置
# =============================================================================
# 设置构建产物的输出目录
# 这样可以保持源码目录的整洁，便于版本控制
set_target_properties(async_log_system async_log_demo
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin      # 可执行文件输出目录
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib      # 共享库输出目录
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib      # 静态库输出目录
)

# =============================================================================
# 构建信息输出
# =============================================================================
# 在配置阶段输出有用的信息
message(STATUS "AsyncLogSystem 构建配置完成")
message(STATUS "项目版本: ${PROJECT_VERSION}")
message(STATUS "C++标准: ${CMAKE_CXX_STANDARD}")
message(STATUS "构建类型: ${CMAKE_BUILD_TYPE}")
message(STATUS "输出目录: ${CMAKE_BINARY_DIR}")
message(STATUS "安装前缀: ${CMAKE_INSTALL_PREFIX}")

# =============================================================================
# 可选配置
# =============================================================================
# 以下是一些可选的配置选项，可以根据需要启用

# 启用地址消毒器（AddressSanitizer）
# 用于调试内存错误，仅在Debug模式下启用
# option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
# if(ENABLE_ASAN AND CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
#     set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")
# endif()

# 启用代码覆盖率
# 用于测试覆盖率分析，仅在Debug模式下启用
# option(ENABLE_COVERAGE "Enable code coverage" OFF)
# if(ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} --coverage")
#     set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")
# endif()

# 启用静态分析
# 使用clang-tidy等工具进行静态代码分析
# option(ENABLE_STATIC_ANALYSIS "Enable static analysis" OFF)
# if(ENABLE_STATIC_ANALYSIS)
#     set(CMAKE_CXX_CLANG_TIDY clang-tidy)
# endif() 