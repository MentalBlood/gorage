cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CXX_FLAGS "-Wall -Wfatal-errors")

SET(PROJECT_NAME gorage)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
	test/main.cpp
  src/random.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include test)
target_link_libraries(${PROJECT_NAME} stdc++fs)
