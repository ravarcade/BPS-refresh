cmake_minimum_required(VERSION 3.0.0)
set(MODULE_NAME toBase64)

set(SOURCES 
    src/main.cpp
)

set(HEADERS 
)

add_executable(${MODULE_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${MODULE_NAME} 
    PRIVATE 
        bps::common)
