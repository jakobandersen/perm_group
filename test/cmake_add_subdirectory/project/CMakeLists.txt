cmake_minimum_required(VERSION 3.10)

project(CMakeTestProject CXX)

add_subdirectory(${PermGroup_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}/PermGroup)

add_executable(doStuff main.cpp)
target_link_libraries(doStuff PermGroup::perm_group)