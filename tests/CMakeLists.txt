enable_testing()

add_executable(tests tests.cpp regexp_tests.cpp)
target_link_libraries(tests formal GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
