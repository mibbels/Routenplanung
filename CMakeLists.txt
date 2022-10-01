############################### Project-Setup ###############################

cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(Routenplanung)


############################### Settings ####################################

set(CMAKE_BUILD_TYPE "Debug")
set(COMPILE_FLAGS "-Wall -Wextra -Wpedantic -Wno-unused-parameter --std=c++17 -g")
set(CORE_LINKER_FLAGS "")
set(APP_LINKER_FLAGS "")


############################### Core ########################################

# source files
file(GLOB_RECURSE SRC_CORE Core/*.cpp)
file(GLOB_RECURSE SRC_EASYLOG Vendor/easylogging/*.cpp)

# compile to a static library
add_library(RouteCore STATIC ${SRC_CORE} ${SRC_EASYLOG})

# include directories
target_include_directories(RouteCore PUBLIC Core)
target_include_directories(RouteCore PUBLIC Vendor/easylogging)
target_include_directories(RouteCore PUBLIC Vendor/stxxl/include)

# core linker flags
target_link_libraries(RouteCore ${CORE_LINKER_FLAGS})


############################### stxxl ########################################

# include the STXXL library in my-project
add_subdirectory(Vendor/stxxl)

# apply STXXL CXXFLAGS to our configuration
set(CMAKE_CXX_FLAGS "${COMPILE_FLAGS} ${STXXL_CXX_FLAGS}")

# add STXXL includes path
include_directories(${STXXL_INCLUDE_DIRS})


############################### Application #################################

# compile to an executable
add_executable(Routenplanung Application/Routenplanung.cpp)

# include directories
target_include_directories(Routenplanung PUBLIC Core)

# link against static libraries
target_link_libraries(Routenplanung RouteCore)
target_link_libraries(Routenplanung ${STXXL_LIBRARIES})

# application linker flags
target_link_libraries(Routenplanung ${APP_LINKER_FLAGS})