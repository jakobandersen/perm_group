if(NOT BUILD_TESTING)
    return()
endif()

add_custom_target(tests DEPENDS ${perm_group_TEST_FILES})

set(sanFlags -fsanitize=undefined -fsanitize=address -fsanitize=leak)
foreach(testName ${perm_group_TEST_FILES})
    add_executable(${testName} EXCLUDE_FROM_ALL ${testName}.cpp)
    target_link_libraries(${testName}
            PRIVATE
            perm_group
            Boost::program_options
            Boost::system
            Threads::Threads)
    if(BUILD_TESTING_SANITIZERS AND NOT BUILD_COVERAGE)
        target_compile_options(${testName} PRIVATE ${sanFlags})
        target_link_libraries(${testName} PRIVATE ${sanFlags})
    endif()
    target_compile_options(${testName} PRIVATE -Wall -Wextra -std=c++14 -pedantic -g -UNDEBUG
            -Wno-unused-parameter
            -Wno-comment
            -Wno-sign-compare
            -Wno-unused-local-typedefs)
    target_add_coverage(${testName})
    add_test(${testName} ${testName})
    add_coverage_case(${testName})
endforeach()

add_subdirectory(cmake_add_subdirectory)
add_subdirectory(cmake_import)