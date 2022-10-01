cmake_minimum_required(VERSION 3.16..3.20 FATAL_ERROR)
# Project name

project(Ui VERSION 0.0.1 LANGUAGES CXX)


include(GNUInstallDirs)

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set(TargetName "${FrameWorkName}.${PROJECT_NAME}")
SET(FRAMEWORK_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
        ${TargetName}
        SHARED
        include/${FrameWorkName}/${PROJECT_NAME}/DllConfig.h
        include/${FrameWorkName}/${PROJECT_NAME}/UiDefs.h
        include/${FrameWorkName}/${PROJECT_NAME}/Geometry.h
        include/${FrameWorkName}/${PROJECT_NAME}/Object.h
        include/${FrameWorkName}/${PROJECT_NAME}/VDC.h
        include/${FrameWorkName}/${PROJECT_NAME}/Console.h
        include/${FrameWorkName}/${PROJECT_NAME}/Painter.h
        include/${FrameWorkName}/${PROJECT_NAME}/Widget.h
	include/${FrameWorkName}/${PROJECT_NAME}/Elements/Label.h
        include/${FrameWorkName}/${PROJECT_NAME}/Elements/Element.h

        cc/Geometry.cc
        cc/Object.cc
        cc/VDC.cc
        cc/Console.cc
        cc/Painter.cc
        cc/Widget.cc
	cc/Elements/Label.cc
        cc/Elements/Element.cc
)

# Only way I found for assemble each of the subproject's include dir.
list(
    APPEND INCLUDE_PATHS
    "${CORE_INCLUDE_DIR}"
#    "${CLOG_INCLUDE_DIR}"
    "${UI_INCLUDE_DIR}"
#    "${DLLOADER_INCLUDE_DIR}"
#    "${XIO_INCLUDE_DIR}"
)
target_compile_definitions(${TargetName} PUBLIC "${TargetName}_DEBUG=$<CONFIG:Debug>")
target_compile_features(${TargetName} PUBLIC cxx_std_20)

include(GenerateExportHeader)
generate_export_header(${TargetName} EXPORT_FILE_NAME ${${TargetName}_export})

IF( EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" )
	EXECUTE_PROCESS( COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
			)
ENDIF()

target_include_directories(${TargetName} PUBLIC
    "$<BUILD_INTERFACE:${INCLUDE_PATHS}>"
        $<INSTALL_INTERFACE:include/${FrameWorkName}/${PROJECT_NAME}>
        )

target_link_libraries(${TargetName} TuxLS.Core ${CMAKE_DL_LIBS} sqlite3)


install(DIRECTORY
        include/${FrameWorkName}/${PROJECT_NAME}/
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include/${FrameWorkName}/${PROJECT_NAME}"
)

install(TARGETS ${TargetName}
		EXPORT ${TargetName}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)
