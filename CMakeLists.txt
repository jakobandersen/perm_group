cmake_minimum_required(VERSION 3.6)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

###########################################################################
# Package Properties
###########################################################################
file(READ "VERSION" PROJECT_VERSION)
string(STRIP "${PROJECT_VERSION}" PROJECT_VERSION) # remove the newline

project(PermGroup VERSION ${PROJECT_VERSION})
set(PermGroup_VERSION ${PROJECT_VERSION} CACHE INTERNAL "" FORCE)
set(PNAME_FILE perm-group) # how the project name should be written in file names
set(CPACK_PACKAGE_CONTACT "Jakob Lykke Andersen <jlandersen@imada.sdu.dk>")


set(CMAKE_CXX_FLAGS_OPTDEBUG "-g -O3")
set(CMAKE_C_FLAGS_OPTDEBUG "-g -O3")
set(CMAKE_EXE_LINKER_FLAGS_OPTDEBUG "")
set(CMAKE_SHARED_LINKER_FLAGS_OPTDEBUG "")
mark_as_advanced(CMAKE_CXX_FLAGS_OPTDEBUG CMAKE_C_FLAGS_OPTDEBUG CMAKE_EXE_LINKER_FLAGS_OPTDEBUG CMAKE_SHARED_LINKER_FLAGS_OPTDEBUG)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
            "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel OptDebug."
            FORCE)
endif()



###########################################################################
# Options
###########################################################################
option(BUILD_DOC "Enable documentation building." ON)
option(BUILD_TESTING "Enable test building." OFF)
option(BUILD_TESTING_SANITIZERS "Compile tests with sanitizers." ON)
option(BUILD_COVERAGE "Enable code coverage." OFF)
enable_testing()  # should be included here to add the targets in the top-level folder

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(BUILD_DOC 0)
    set(BUILD_TESTING 0)
    set(BUILD_COVERAGE 0)
endif()


###########################################################################
# Individual checks
###########################################################################
set(CMAKE_CXX_STANDARD 14)


# Boost
# -------------------------------------------------------------------------
set(v 1.67.0)
if(BUILD_TESTING)
    find_package(Boost ${v} REQUIRED COMPONENTS program_options unit_test_framework system)
else()
    find_package(Boost ${v} REQUIRED)
endif()


# Sage
# -------------------------------------------------------------------------
if(BUILD_TESTING)
    find_program(SAGE sage)
    if(SAGE)
        message(STATUS "Sage: ${SAGE}")
        set(PERM_GROUP_SAGE ${SAGE})  # for Config.hpp substitution
    else()
        message(FATAL_ERROR "Sage not found, required for tests. Use -DBUILD_TESTING=off to disalble.")
    endif()
endif()


# Sphinx
# -------------------------------------------------------------------------
if(BUILD_DOC)
    find_program(SPHINX NAMES sphinx-build sphinx-build3 sphinx-build-3 sphinx-build2)
    if(SPHINX)
        message(STATUS "Sphinx: ${SPHINX}")
    else()
        message(FATAL_ERROR "Sphinx not found, required for building the documentation. Use --DBUILD_DOC=off to disable.")
    endif()
endif()


# Threads
# -------------------------------------------------------------------------
if(BUILD_TESTING)
    find_package(Threads REQUIRED)
endif()


###########################################################################
# Targets and Artefacts
###########################################################################
include(CMakeFiles.txt)
include(Coverage)
include(GNUInstallDirs)


# libperm_group
# -------------------------------------------------------------------------
add_library(perm_group
        SHARED
            ${perm_group_INCLUDE_FILES}
            ${perm_group_SRC_FILES})
add_library(PermGroup::perm_group ALIAS perm_group)
target_include_directories(perm_group
        PUBLIC
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
            ${Boost_INCLUDE_DIR}) # we only use header-only libs from Boost, right?
target_compile_options(perm_group PRIVATE -Wall -Wextra
        -Wno-unused-parameter
        -Wno-comment
        -Wno-sign-compare
        -Wno-unused-local-typedefs
        -Wno-parentheses
        $<$<CXX_COMPILER_ID:GNU>:-Wno-deprecated-copy>)
set_target_properties(perm_group PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
target_compile_definitions(perm_group PRIVATE PERM_GROUP_SOURCE)

target_add_coverage(perm_group)

install(TARGETS perm_group
        EXPORT PROJECT_exports
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT PermGroup_lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT PermGroup_lib
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT PermGroup_lib)
install(DIRECTORY include/perm_group
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT PermGroup_dev
        FILES_MATCHING PATTERN "*.hpp")


# config.hpp
# -------------------------------------------------------------------------
configure_file(config.hpp.in include/perm_group/config.hpp @ONLY)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/perm_group
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT PermGroup_dev)


# Subdirs
# -------------------------------------------------------------------------
add_subdirectory(doc)
add_subdirectory(test)


# CMake Package Support
# -------------------------------------------------------------------------
include(CMakePackageConfigHelpers)
# due to the stupid restrictive version formatting we change it
set(exportDir ${CMAKE_CURRENT_BINARY_DIR}/lib/cmake/${PROJECT_NAME})
write_basic_package_version_file(
        ${exportDir}/${PROJECT_NAME}Config-version.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)
install(EXPORT PROJECT_exports
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
        FILE ${PROJECT_NAME}Config.cmake)
install(DIRECTORY ${exportDir}
        DESTINATION lib/cmake
        COMPONENT PermGroup_dev)


# pkg-config file
# -------------------------------------------------------------------------
set(exportDir ${CMAKE_CURRENT_BINARY_DIR}/lib/pkgconfig)
configure_file(lib/pkgconfig/perm-group.pc.in ${exportDir}/perm-group.pc @ONLY)
install(FILES ${exportDir}/perm-group.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT PermGroup_dev)


# Packaging
# -------------------------------------------------------------------------
string(TOUPPER ${PROJECT_NAME} PNAME_UPPER)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})  # because the default doesn't include the tweak number
set(CPACK_GENERATOR "DEB")
#set(CPACK_GENERATOR "DEB" "RPM")
set(CPACK_DEB_COMPONENT_INSTALL ON)
#set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)

# we use CACHE INTERNAL to make it work when in a subproject
set(CPACK_DEBIAN_${PNAME_UPPER}_RUN_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_LIB_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DEV_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DOC_PACKAGE_SHLIBDEPS ON                                          CACHE INTERNAL "")

set(CPACK_COMPONENT_${PNAME_UPPER}_RUN_DEPENDS ${PNAME_UPPER}_LIB                                 CACHE INTERNAL "")
set(CPACK_COMPONENT_${PNAME_UPPER}_DEV_DEPENDS ${PNAME_UPPER}_LIB                                 CACHE INTERNAL "")

set(CPACK_DEBIAN_${PNAME_UPPER}_RUN_PACKAGE_NAME    "${PNAME_FILE}"                               CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_LIB_PACKAGE_NAME "lib${PNAME_FILE}"                               CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DEV_PACKAGE_NAME "lib${PNAME_FILE}-dev"                           CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DOC_PACKAGE_NAME    "${PNAME_FILE}-doc"                           CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_RUN_FILE_NAME       "${PNAME_FILE}_${PROJECT_VERSION}.deb"        CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_LIB_FILE_NAME    "lib${PNAME_FILE}_${PROJECT_VERSION}.deb"        CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DEV_FILE_NAME    "lib${PNAME_FILE}-dev_${PROJECT_VERSION}.deb"    CACHE INTERNAL "")
set(CPACK_DEBIAN_${PNAME_UPPER}_DOC_FILE_NAME       "${PNAME_FILE}-doc_${PROJECT_VERSION}.deb"    CACHE INTERNAL "")

#set(CPACK_RPM_${PNAME_UPPER}_RUN_PACKAGE_NAME       "${PNAME_FILE}"                              CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_LIB_PACKAGE_NAME    "lib${PNAME_FILE}"                              CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DEV_PACKAGE_NAME    "lib${PNAME_FILE}-devel"                        CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DOC_PACKAGE_NAME       "${PNAME_FILE}-doc"                          CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_RUN_FILE_NAME          "${PNAME_FILE}-${PROJECT_VERSION}.rpm"       CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_LIB_FILE_NAME       "lib${PNAME_FILE}-${PROJECT_VERSION}.rpm"       CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DEV_FILE_NAME       "lib${PNAME_FILE}-devel-${PROJECT_VERSION}.rpm" CACHE INTERNAL "")
#set(CPACK_RPM_${PNAME_UPPER}_DOC_FILE_NAME          "${PNAME_FILE}-doc-${PROJECT_VERSION}.rpm"   CACHE INTERNAL "")

if(NOT hasParent)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME
            "${PNAME_FILE}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_SOURCE_GENERATOR "TGZ")
    set(CPACK_SOURCE_IGNORE_FILES
            ".swp$"
            "~$"
            "__pycache__"
            *.pyc
            "/.git"
            "/.idea"
            "/build"
            "/stage"
            "/bootstrap.sh")
    add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
    include(CPack)
endif()
