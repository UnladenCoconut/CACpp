#checkout https://github.com/ttroy50/cmake-examples

cmake_minimum_required(VERSION 3.24)

project(CACpp C CXX) #C needed for GLAD

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/ssq/)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/lib/)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib/)

set(SOURCES
${CMAKE_SOURCE_DIR}/src/cfg.hpp
${CMAKE_SOURCE_DIR}/src/main.cpp
${CMAKE_SOURCE_DIR}/src/menu.hpp
${CMAKE_SOURCE_DIR}/src/server.hpp
${CMAKE_SOURCE_DIR}/src/system.hpp
${CMAKE_SOURCE_DIR}/src/username.hpp
)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main ${SOURCES})
target_include_directories(main PUBLIC ${CMAKE_SOURCE_DIR}/src/ )
include_directories(lib/ssq/include)
target_link_libraries(main PUBLIC ssq kernel32 Advapi32 Userenv ws2_32 Dnsapi)
