project(AIMPBS)

cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread -g -Wall -ldl")

include_directories("./*.h")
file(GLOB SOURCES "./*.cpp")

# change this to location of cplex studio eg. /usr/programfiles/CPLEX_Studio1210
set(PATH_TO_CPLEX_STUDIO      "/////////change here///////////")


set(CPLEX_INCLUDE_DIRS        "${PATH_TO_CPLEX_STUDIO}/cplex/include")
set(CONCERT_INCLUDE_DIRS      "${PATH_TO_CPLEX_STUDIO}/concert/include")
set(CPLEX_LIBRARIES           "${PATH_TO_CPLEX_STUDIO}/cplex/lib/x86-64_linux/static_pic/libcplex.a")
set(ILOCPLEX_LIBRARIES        "${PATH_TO_CPLEX_STUDIO}//cplex/lib/x86-64_linux/static_pic/libilocplex.a")
set(CPLEXDISTMIP_LIBRARIES    "${PATH_TO_CPLEX_STUDIO}/cplex/lib/x86-64_linux/static_pic/libcplexdistmip.a")
set(CONCERT_LIBRARIES         "${PATH_TO_CPLEX_STUDIO}//concert/lib/x86-64_linux/static_pic/libconcert.a")



add_executable(AIMPBS ${SOURCES})
set_property(
  TARGET AIMPBS 
  PROPERTY COMPILE_DEFINITIONS IL_STD
)

# Find Boost
find_package(Boost 1.49.0 REQUIRED COMPONENTS program_options system filesystem)
target_include_directories(AIMPBS PUBLIC ${Boost_INCLUDE_DIRS} ${CPLEX_INCLUDE_DIRS} ${CONCERT_INCLUDE_DIRS})
#include_directories(${Boost_INCLUDE_DIRS})


target_link_libraries(AIMPBS ${Boost_LIBRARIES}  ${ILOCPLEX_LIBRARIES} ${CONCERT_LIBRARIES} ${CPLEX_LIBRARIES} ${CPLEXDISTMIP_LIBRARIES}  ${CMAKE_DL_LIBS})
set (target_link_options "-lilocplex -lconcert -lcplex -lm -lpthread")
