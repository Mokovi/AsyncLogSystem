cmake_minimum_required(VERSION 3.15)
project(LogSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# 依赖库
find_package(Threads REQUIRED)

# 源文件
set(SOURCES
    src/log_types.cpp
    src/log_system.cpp
)

# 头文件
set(HEADERS
    include/log_types.h
    include/log_system.h
)

# 主库
add_library(log_system ${SOURCES} ${HEADERS})
target_include_directories(log_system PUBLIC include)
target_link_libraries(log_system Threads::Threads)

# 主程序
add_executable(log_system_main src/main.cpp)
target_link_libraries(log_system_main log_system)

# 测试
enable_testing()
add_subdirectory(tests)

# 示例
add_subdirectory(examples)

# 安装规则
install(TARGETS log_system
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS} DESTINATION include/log_system)
