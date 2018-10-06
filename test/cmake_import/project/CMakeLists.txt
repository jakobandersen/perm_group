cmake_minimum_required(VERSION 3.10)

project(CMakeTestProject CXX)

find_package(PermGroup REQUIRED)

add_executable(doStuff main.cpp)
target_link_libraries(doStuff PermGroup::perm_group)