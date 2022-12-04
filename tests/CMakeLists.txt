enable_testing()

aux_source_directory(. TESTS_SOURCES)
add_executable(tests ${TESTS_SOURCES})
target_link_libraries(tests formal GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
