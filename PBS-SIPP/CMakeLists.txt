project(AIMPBS)

cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread -g -Wall -ldl")
set(CMAKE_CXX_STANDARD 14)

include_directories("./*.h")
file(GLOB SOURCES "./*.cpp")

# Find Boost
find_package(Boost 1.49.0 REQUIRED COMPONENTS program_options system filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(AIMPBS ${SOURCES})
target_link_libraries(AIMPBS ${Boost_LIBRARIES})