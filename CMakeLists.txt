cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)

SET(PROJECT_NAME gorage)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
	test/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include test)
