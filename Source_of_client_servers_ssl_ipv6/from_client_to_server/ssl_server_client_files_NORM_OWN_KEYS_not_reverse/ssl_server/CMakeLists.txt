cmake_minimum_required(VERSION 3.14)
project(ssl_server_receive)

set(CMAKE_CXX_STANDARD 14)


link_directories(/usr/lib/x86_64-linux-gnu/)

link_libraries(boost_system.so)
link_libraries(crypto.so)
link_libraries(ssl.so)
link_libraries(pthread.so)

add_executable(ssl_server_receive main.cpp)