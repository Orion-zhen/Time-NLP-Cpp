cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PROJECT_NAME TimeCpp)
set(SrcList Time-Cpp.cpp jsoncpp.cpp LunarSolatConverter.cpp StringPreHandler.cpp TimeNormalizer.cpp TimeUnit.cpp)
set(HeaderList share.h)

project(${PROJECT_NAME})
# add_library(${PROJECT_NAME}lib STATIC ${SrcList} ${HeaderList})
add_executable(${PROJECT_NAME} ${SrcList})