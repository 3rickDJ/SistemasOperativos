cmake_minimum_required(VERSION 3.25)
project(project2)
file(GLOB SOURCES src/*.cpp)
include_directories( ${CMAKE_SOURCE_DIR}/include )

set(CMAKE_CXX_STANDARD 17)

#find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)

add_executable(project2 ${SOURCES})
#target_link_libraries(project2 PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(project2 PRIVATE fmt::fmt)


