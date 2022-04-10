set(This ${MODULE_NAME}_test) # will be client_test. MODULE_NAME is defined in partent cmake

set(TESTSRC
    src/base64_tests.cpp
    src/to_vector_tests.cpp
    src/logger_tests.cpp
)

add_executable(${This} ${TESTSRC})
target_link_libraries(${This}  
    ${MODULE_NAME}
    GTest::gmock GTest::gmock_main
    GTest::gtest GTest::gtest_main
)
