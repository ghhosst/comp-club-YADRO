cmake_minimum_required(VERSION 3.2)

project(compClub.out LANGUAGES CXX)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 11) 
set(CMAKE_BUILD_TYPE Debug)


set(SOURCES 
    src/main.cpp
    src/ComputerClub.cpp
)

set(HEADERS
    include/ComputerClub.h
)



include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_compile_options(compClub.out PRIVATE -g)