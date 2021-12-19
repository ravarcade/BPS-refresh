add_executable(ravtest src/main.cpp)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(ravtest PRIVATE unofficial::sqlite3::sqlite3)
