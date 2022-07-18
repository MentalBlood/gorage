cmake_minimum_required(VERSION 3.0)

SET(PROJECT_NAME gorage)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} 

	example.cpp

	src/Cache.cpp
	src/Storage.cpp
	src/ItemStorage.cpp
	src/FileStorage.cpp
	src/BinaryFileStorage.cpp

)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "-std=c++17") 
target_link_libraries(${PROJECT_NAME} -static-libstdc++ -std=c++17)