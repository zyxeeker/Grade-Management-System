cmake_minimum_required(VERSION 3.16)
project(webSever)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS "-lmysqlclient -lpthread -lm -lrt -latomic -ldl")

add_executable(webSever main.cpp sql/sql.h sql/sql.cpp log/log.h log/log.cpp http/http.h http/http.cpp thread/thread.h)

target_link_libraries(webSever libmysqlclient.so)
