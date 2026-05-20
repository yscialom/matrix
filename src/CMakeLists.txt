add_library(ysc-matrix INTERFACE)
add_library(ysc::matrix ALIAS ysc-matrix)
set_target_properties(ysc-matrix PROPERTIES EXPORT_NAME matrix)
target_include_directories(ysc-matrix INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/ysc>
)
target_compile_features(ysc-matrix INTERFACE cxx_std_20)

##
# Install
#
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS ysc-matrix
    EXPORT ysc-matrixTargets
)

install(EXPORT ysc-matrixTargets
    FILE ysc-matrixTargets.cmake
    NAMESPACE ysc::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ysc-matrix
)

install(FILES
    include/matrix.hpp
    include/matrix_view.hpp
    include/matrix_detail.hpp
    DESTINATION include/ysc
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ysc-matrixConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ysc-matrixConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ysc-matrix
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ysc-matrixConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ysc-matrixConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ysc-matrixConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ysc-matrix
)
