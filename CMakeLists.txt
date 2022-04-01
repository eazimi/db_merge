cmake_minimum_required(VERSION 3.5)

set(PROJECT db_merge)
project(${PROJECT} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR src)

add_executable(${PROJECT}
    ${SRC_DIR}/main.cpp
  )