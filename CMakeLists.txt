cmake_minimum_required(VERSION 3.10)

project(test_proj1)

set(CMAKE_AUTOMOC ON)

list(APPEND CMAKE_PREFIX_PATH "C:/IT_libs/Qt/6.4.1/msvc2019_64/bin")

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Charts)

add_executable(test_proj1 test_proj1.cpp "Qtapp.cpp" "Qtapp.h" "udpOn.cpp" "udpOn.h" "kih-alg.cpp" "kih-alg.h" "bih-alg.cpp" "bih-alg.h")

target_link_libraries(test_proj1 PRIVATE Qt6::Core)
target_link_libraries(test_proj1 PRIVATE Qt6::Widgets)
target_link_libraries(test_proj1 PRIVATE Qt6::Charts)

if(WIN32)
    target_link_libraries(test_proj1 PRIVATE ws2_32)
endif()
