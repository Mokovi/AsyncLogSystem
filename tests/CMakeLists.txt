# 查找Google Test
find_package(GTest REQUIRED)

# 基础功能测试
add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic log_system GTest::gtest GTest::gtest_main)

# 异步功能测试
add_executable(test_async test_async.cpp)
target_link_libraries(test_async log_system GTest::gtest GTest::gtest_main)

# 添加测试
add_test(NAME BasicTests COMMAND test_basic)
add_test(NAME AsyncTests COMMAND test_async)

# 设置测试属性
set_tests_properties(BasicTests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)

set_tests_properties(AsyncTests PROPERTIES
    TIMEOUT 60
    LABELS "async"
)
