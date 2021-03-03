cmake_minimum_required(VERSION 3.16)
project(webSever)

set(CMAKE_CXX_STANDARD 11)
find_package(yaml-cpp)

#set(CMAKE_CXX_FLAGS "-lmysqlclient -lpthread -lm -lrt -latomic -ldl")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin) # 二进制文件的输出目录

add_executable(${PROJECT_NAME} main.cpp sql/sql.h sql/sql.cpp log/log.h log/log.cpp http/http.h thread/thread.h http/http.cpp locker/locker.h thread/thread.cpp conn/conn.h conn/conn.cpp)

target_link_libraries(${PROJECT_NAME} libmysqlclient.so)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/yaml-cpp)
target_link_libraries(${PROJECT_NAME} ${YAML_CPP_LIBRARIES})