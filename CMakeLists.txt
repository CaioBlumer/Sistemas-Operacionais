cmake_minimum_required (VERSION 2.6)
project(SO)



set(CMAKE_CXX_FLAGS "-lpthread -g -ggdb -Wall -fpermissive ")
#set(CMAKE_CXX_FLAGS "-O3")
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)



add_executable(server server.c)
add_executable(cliente cliente.c)
