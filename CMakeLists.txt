cmake_minimum_required(VERSION 3.12)
project(msh C)

set(CMAKE_C_STANDARD 11)
include_directories(/usr/include/readline)

add_executable(msh main.c)
target_link_libraries(msh readline)