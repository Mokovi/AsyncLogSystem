# 查找Google Test
find_package(GTest REQUIRED)

# 测试可执行文件
add_executable(test_basic test_basic.cpp)
target_link_libraries(test_basic log_system GTest::gtest GTest::gtest_main)

# 添加测试
add_test(NAME BasicTests COMMAND test_basic)

# 设置测试属性
set_tests_properties(BasicTests PROPERTIES
    TIMEOUT 30
    LABELS "unit"
)
