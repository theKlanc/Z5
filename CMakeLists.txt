cmake_minimum_required(VERSION 2.8)

project(Z3)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")
find_package(Switch REQUIRED)
add_executable(${PROJECT_NAME} "main.cpp")
