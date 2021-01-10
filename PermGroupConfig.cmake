get_filename_component(perm_group_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)
list(APPEND CMAKE_MODULE_PATH ${perm_group_CMAKE_DIR})

@perm_group_config_dependencies@

include("${perm_group_CMAKE_DIR}/PermGroupTargets.cmake")