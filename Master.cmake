cmake_minimum_required(VERSION 3.13..3.22 FATAL_ERROR)
## Project name

message ("**************** " ${FrameWorkName} " Master Project ********************")
#
option(BUILD_TUXLS_ELEMENTS  "Enable building the Root elements library" ON)
#option(BUILD_TUXLS_LOGGER "Enable building the TuxLSLogger library (TuxLS.Logger) library" ON)
option(BUILD_TUXLS_UI "Enable building the Console UI (TuxLS.UI)library" ON)
#option(BUILD_TUXLS_PLUGIN "Enable building the dynamic lib loader (TuxLS.Plugin)library" OFF)
#option(BUILD_TUXLS_SCRIPTURE "Enable building the dynamic lib loader (Scripture)library" ON)
option(BUILD_TESTS "Enable building the global tests (Test.app) application " ON)
#

SET(FRAMEWORK_ROOT ${CMAKE_CURRENT_SOURCE_DIR})


message("${FrameWorkName} Root Directory:" ${FRAMEWORK_ROOT})



if(BUILD_TUXLS_ELEMENTS)
    SET(CORE_INCLUDE_DIR ${FRAMEWORK_ROOT}/Core/include)
    add_subdirectory(Core)
    message ("TuxLS.Core library:" "libTuxLS.Core.{so/dll/dylib} set -> ${CORE_INCLUDE_DIR}")
endif()


#
#if(BUILD_TUXLS_LOGGER)
#    SET(LOGGER_INCLUDE_DIR ${FRAMEWORK_ROOT}/Logger/include)
#    add_subdirectory(Logger)
#    message ("submodule build:" "libTuxLS.Logger.{so/dll/dylib} set -> ${LOGGER_INCLUDE_DIR}")
#endif()

if(BUILD_TUXLS_UI)
    SET(UI_INCLUDE_DIR ${FRAMEWORK_ROOT}/Ui/include)
    add_subdirectory(Ui)
    message ("library build:" "libTuxLS.Ui.{so/dll/dylib} set -> ${UI_INCLUDE_DIR}")
endif()
#
#if(BUILD_TUXLS_LOADER)
#    SET(TUXLS_LOADER_INCLUDE_DIR ${FRAMEWORK_ROOT}/TuxLS.Plug/include)
#    add_subdirectory(TuxLS.Plug)
#    message ("submodule build:" "libTuxLS.Plug.{so/dll/dylib} set -> ${TUXLS_LOADER_INCLUDE_DIR}")
#endif()
#
#if(BUILD_TUXLS_SCRIPTURE)
#    SET(TUXLS_SCRIPTURE_INCLUDE_DIR ${FRAMEWORK_ROOT}/TuxLS.Scripture/include)
#    add_subdirectory(TuxLS.Scripture)
#    message ("submodule build:" "libTuxLS.Scripture.{so/dll/dylib} set -> ${TUXLS_SCRIPTURE_INCLUDE_DIR}")
#endif()
#
if(BUILD_TESTS)
    add_subdirectory(Tests)
    message ("test application build set to ON")
endif()

#
#
#IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
#	EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
#			${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
#			)
#ENDIF()
#
#
## uninstall target
#if(NOT TARGET uninstall)
#  configure_file(
#    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
#    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
#    IMMEDIATE @ONLY)
#
#  add_custom_target(uninstall
#    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
#endif()
