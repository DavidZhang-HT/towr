# FindMeshcatCpp.cmake
# Find the MeshCat-cpp library
#
# This module defines:
#  MeshcatCpp_FOUND - True if MeshCat-cpp is found
#  MeshcatCpp_INCLUDE_DIRS - Include directories for MeshCat-cpp
#  MeshcatCpp_LIBRARIES - Libraries to link against
#  MeshcatCpp::MeshcatCpp - Imported target for MeshCat-cpp

find_path(MeshcatCpp_INCLUDE_DIR
    NAMES MeshcatCpp/Meshcat.h
    PATHS
        /usr/local/include
        /usr/include
        ${CMAKE_INSTALL_PREFIX}/include
    PATH_SUFFIXES
        MeshcatCpp
)

find_library(MeshcatCpp_LIBRARY
    NAMES MeshcatCpp meshcat-cpp
    PATHS
        /usr/local/lib
        /usr/lib
        ${CMAKE_INSTALL_PREFIX}/lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MeshcatCpp
    REQUIRED_VARS MeshcatCpp_LIBRARY MeshcatCpp_INCLUDE_DIR
)

if(MeshcatCpp_FOUND)
    set(MeshcatCpp_INCLUDE_DIRS ${MeshcatCpp_INCLUDE_DIR})
    set(MeshcatCpp_LIBRARIES ${MeshcatCpp_LIBRARY})
    
    if(NOT TARGET MeshcatCpp::MeshcatCpp)
        add_library(MeshcatCpp::MeshcatCpp UNKNOWN IMPORTED)
        set_target_properties(MeshcatCpp::MeshcatCpp PROPERTIES
            IMPORTED_LOCATION "${MeshcatCpp_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${MeshcatCpp_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(MeshcatCpp_INCLUDE_DIR MeshcatCpp_LIBRARY)
