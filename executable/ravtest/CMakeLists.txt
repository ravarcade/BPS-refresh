cmake_minimum_required(VERSION 3.0.0)
set(MODULE_NAME ravtest)

set(SOURCES 
    src/main.cpp
)

set(HEADERS 
    # include/importer/tmp.hpp
)

add_executable(${MODULE_NAME} ${SOURCES} ${HEADERS})
find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(ravtest 
    PRIVATE 
        bps::importer
        bps::engine
        unofficial::sqlite3::sqlite3)
