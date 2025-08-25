# =============================================================================
# AsyncLogSystem 示例程序构建配置
# =============================================================================
# 
# 功能说明:
# - 构建基础使用示例程序
# - 构建高级使用示例程序
# - 演示日志系统的各种功能
# - 提供学习和测试的参考代码
# =============================================================================

# 基础使用示例程序
add_executable(basic_usage_example basicUsage.cpp)
target_link_libraries(basic_usage_example async_log_system)
target_include_directories(basic_usage_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 高级使用示例程序
add_executable(advanced_usage_example advancedUsage.cpp)
target_link_libraries(advanced_usage_example async_log_system)
target_include_directories(advanced_usage_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 装饰器示例程序
add_executable(decorator_example decoratorExample.cpp)
target_link_libraries(decorator_example async_log_system)
target_include_directories(decorator_example PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 设置输出目录
set_target_properties(basic_usage_example advanced_usage_example decorator_example
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/examples
)

# 输出构建信息
message(STATUS "Examples directory configured - added basic and advanced usage examples")
message(STATUS "  - basic_usage_example: 基础使用示例")
message(STATUS "  - advanced_usage_example: 高级使用示例")
