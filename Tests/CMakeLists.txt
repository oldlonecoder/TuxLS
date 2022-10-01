cmake_minimum_required(VERSION 3.16)

#I keep bare minimum of the QT5 or 6+ components framework access in cmake.
# They are disabled in this template.

project(Test.app VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

#find_package(Qt5 COMPONENTS Widgets REQUIRED)


# Only way I found for assemble each of the subproject's include dir.
list(
    APPEND INCLUDE_PATHS
    "${CORE_INCLUDE_DIR}"
    "${UI_INCLUDE_DIR}"
#    "${DLLOADER_INCLUDE_DIR}"
#    "${XIO_INCLUDE_DIR}"
)

add_executable(
    ${PROJECT_NAME}
    Test.cc
)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${INCLUDE_PATHS}>"
        )


target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} sqlite3 TuxLS.Core TuxLS.Ui)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin )

#INCLUDE(Tests.cmake)
