set(This ${MODULE_NAME}_test)

set(TESTSRC
    src/imageDecode_tests.cpp
)

add_executable(${This} ${TESTSRC})
target_link_libraries(${This}  
    ${MODULE_NAME}
    fmt::fmt
    GTest::gmock GTest::gmock_main
    GTest::gtest GTest::gtest_main
)
