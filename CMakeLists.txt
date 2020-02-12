cmake_minimum_required (VERSION 3.1)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_COMPILER "g++")
project(store_cashier)
add_compile_options(-g)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../cadmium/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../DESTimes/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

file(MAKE_DIRECTORY simulation_results)

add_executable(test_client_generator test/main_client_generator_test.cpp)
add_executable(simulator top_model/main.cpp)

target_link_libraries(test_client_generator Boost::program_options)
target_link_libraries(simulator Boost::program_options)
