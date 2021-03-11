cmake_minimum_required(VERSION 3.16)
project(webSever)

set(CMAKE_CXX_STANDARD 11)
find_package(yaml-cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin) # 二进制文件的输出目录

add_executable(${PROJECT_NAME} main.cpp sql/sql.h sql/sql.cpp log/log.h log/log.cpp http/http.h thread/thread.h http/http.cpp locker/locker.h conn/conn.h conn/conn.cpp server.cpp server.h)

target_link_libraries(${PROJECT_NAME} libmysqlclient.so)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/yaml-cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/libyaml-cpp.a)