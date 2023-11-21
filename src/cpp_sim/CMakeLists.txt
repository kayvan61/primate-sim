cmake_minimum_required(VERSION 3.10)
project(PrimSim)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS program_options regex REQUIRED)

file(GLOB SOURCES "src/*.cpp")

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_executable(PrimSim ${SOURCES})
  include_directories(include)
  target_link_libraries(PrimSim ${Boost_LIBRARIES})
endif()

set(CMAKE_C_FLAGS "-Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
