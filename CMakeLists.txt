cmake_minimum_required(VERSION 3.15)
# 指定所需最低 CMake 版本。若低于此版本，CMake 会报错并提示升级。
# 选择 3.15 能支持比较多较新的 CMake 特性；根据项目需要可调整。

project(LogSystem VERSION 1.0.0 LANGUAGES CXX)
# 定义工程名为 LogSystem，版本号 1.0.0，表明本工程使用 C++ 语言。
# 之后可以通过 ${PROJECT_NAME} 和 ${PROJECT_VERSION} 访问这些值。

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 要求使用的 C++ 标准为 C++17，并且强制要求开启（如果不支持会失败）。

# 编译选项（全局）
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
# 这里通过修改全局变量设置编译器选项：
# -Wall -Wextra: 开启大部分警告
# DEBUG: debug 配置下加 -g、禁用优化（-O0），并定义 DDEBUG 宏
# RELEASE: release 配置下优化为 -O3，并定义 DNDEBUG 宏（通常用于屏蔽 assert）
# 或使用 CMAKE_CXX_FLAGS_DEBUG/CMAKE_CXX_FLAGS_RELEASE 只是作为兼容写法。

# 依赖库
find_package(Threads REQUIRED)
# 查找线程库（通常是 pthread）。如果找不到，会停止并报错（REQUIRED）。
# 找到后我们会使用 Threads::Threads 来链接 target（见下方）。

# 源文件
set(SOURCES
    src/log_types.cpp
    src/log_system.cpp
)
# 把所有源文件列在一个变量中，方便复用（如创建库和测试时都用同一份源文件列表）。

# 头文件
set(HEADERS
    include/log_types.h
    include/log_system.h
)
# 这里把头文件也放到变量中，便于安装时统一拷贝（见后面的 install(FILES ...)）。

# 主库（静态/共享由构建配置决定）
add_library(log_system ${SOURCES} ${HEADERS})
# 创建一个库 target 名为 log_system，包含前面列出的源和头。
# 注意：把头文件列在 add_library 中不是必须的，但这样在 IDE（如 Visual Studio）中会显示头文件，方便开发。

target_include_directories(log_system PUBLIC include)
# 为 log_system 指定 include 目录。PUBLIC 表示：
# - 使用该库的 target 也会继承这个 include 路径（等同于安装后用户可以包含 <log_system/...>）。
# 可替换为 PRIVATE（仅本 target）或 INTERFACE（仅传递给使用者）。

target_link_libraries(log_system Threads::Threads)
# 链接所需的外部库，这里链接线程库（由 find_package(Threads) 提供）。
# 若 log_system 依赖其他库，也在此处添加。

# 主程序（可执行文件）
add_executable(log_system_main src/main.cpp)
# 创建一个可执行 target，用于运行或演示库功能。

target_link_libraries(log_system_main log_system)
# 将主程序与上面创建的库链接。
# 这里没有显式设置 include 目录因为 log_system 的 PUBLIC include 已经传递给使用者（可执行）。

# 测试
enable_testing()
# 启用 CTest 支持，使 add_test 可用，并允许通过 ctest 运行测试。

add_subdirectory(tests)
# 将 tests 目录作为子目录处理。常见做法是 tests/CMakeLists.txt 中定义单元测试 target（比如使用 GTest）。
# 如果子目录中没有 CMakeLists.txt，会报错。

# 示例
add_subdirectory(examples)
# 将 examples 目录作为子目录处理，常用于放置 demo 或 sample 程序。

# 安装规则（用于 `make install` 或 `ninja install`）
install(TARGETS log_system
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
# 为库设置安装位置：
# - LIBRARY: 共享库（.so/.dylib）安装目录（unix）
# - ARCHIVE: 静态库（.a）安装目录
# - RUNTIME: 可执行文件安装目录（主要用于 Windows 的 DLL 或 exe）
# 如果你想把共享库安装到 lib/${PROJECT_NAME} 或 lib64，可在此处做更细分设置。
# 若希望安装时同时导出 CMake 配置文件以便 find_package 使用，需要额外写 install(EXPORT ...) 和 configure_package_config_file。

install(FILES ${HEADERS} DESTINATION include/log_system)
# 将头文件拷贝到安装目录的 include/log_system 下，便于外部使用者通过 #include <log_system/log_system.h> 使用。
# 若头文件组织较复杂，可以使用 install(DIRECTORY ...) 来安装整个 include 目录（保留子目录结构）。
