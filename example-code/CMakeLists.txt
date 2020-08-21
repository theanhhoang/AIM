project(cbs)

cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -Wall")
set(CMAKE_CXX_STANDARD 14)


set(CPLEX_INCLUDE_DIRS      "/Applications/CPLEX_Studio128/cplex/include")
set(CPLEX_LIBRARIES         "/Applications/CPLEX_Studio128/cplex/lib/x86-64_osx/static_pic/libcplex.a")


include_directories("inc")
file(GLOB SOURCES "src/*.cpp")

# Find Boost
find_package( Boost 1.49.0 REQUIRED COMPONENTS program_options system filesystem)
include_directories( ${Boost_INCLUDE_DIRS} )

include_directories(${CPLEX_INCLUDE_DIRS})
include_directories("/Applications/CPLEX_Studio128/concert/include" )

add_executable(cbs ${SOURCES} )

set_property(
  TARGET cbs 
  PROPERTY COMPILE_DEFINITIONS IL_STD
  )


target_link_libraries(cbs ${Boost_LIBRARIES})

target_link_libraries(  cbs PRIVATE ${CPLEX_LIBRARIES})
target_link_libraries(  cbs PRIVATE "/Applications/CPLEX_Studio128/concert/lib/x86-64_osx/static_pic/libconcert.a")
target_link_libraries(  cbs PRIVATE "/Applications/CPLEX_Studio128/cplex/lib/x86-64_osx/static_pic/libilocplex.a")

