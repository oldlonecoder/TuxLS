cmake_minimum_required(VERSION 3.16..3.20 FATAL_ERROR)
# Project name

project(Core VERSION 0.0.1 LANGUAGES CXX)


include(GNUInstallDirs)

SET( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set(TargetName "${FrameWorkName}.${PROJECT_NAME}")
SET(FRAMEWORK_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
        ${TargetName}
        SHARED
        include/${FrameWorkName}/${PROJECT_NAME}/String.h
        include/${FrameWorkName}/${PROJECT_NAME}/CString.h
        include/${FrameWorkName}/${PROJECT_NAME}/Rem.h
        include/${FrameWorkName}/${PROJECT_NAME}/Expect.h
	include/${FrameWorkName}/${PROJECT_NAME}/Encodings/TextAttr.h
	include/${FrameWorkName}/${PROJECT_NAME}/Encodings/Icons.h
	include/${FrameWorkName}/${PROJECT_NAME}/CString.h
	include/${FrameWorkName}/${PROJECT_NAME}/Encodings/Accent.h
    include/${FrameWorkName}/${PROJECT_NAME}/Encodings/Text.h
	cc/Icons.cc
        cc/String.cc
	cc/TextAttr.cc
	cc/CString.cc
	cc/Rem.cc
	cc/Accent.cc
    cc/Text.cc
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
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        $<INSTALL_INTERFACE:include/${FrameWorkName}/${PROJECT_NAME}>
        )

target_link_libraries(${TargetName} ${CMAKE_DL_LIBS} sqlite3)


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
