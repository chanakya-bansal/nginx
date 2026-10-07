cmake_minimum_required(VERSION 3.16)
project(nginx CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -O2)

add_executable(nginx
    src/main.cpp
    src/server.cpp
    src/http_parser.cpp
    src/http_response.cpp
)

target_include_directories(nginx PRIVATE src)