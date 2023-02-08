cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
# set (CMAKE_CXX_FLAGS "/std:c++17")

SET(PROJECT_NAME gorage)
SET(TEST_NAME ${PROJECT_NAME}__test)

# add_compile_definitions(WINDOWS)

project(${PROJECT_NAME})

add_library(${PROJECT_NAME}
	include/gorage.cpp
)


project(${TEST_NAME})

add_executable(${TEST_NAME}
	test/main.cpp
)

target_include_directories(${TEST_NAME} PRIVATE include test)