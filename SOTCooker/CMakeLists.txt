cmake_minimum_required(VERSION 3.5)



if(UNIX)
    message( FATAL_ERROR "Linux is not supported, CMake will exit." )
endif(UNIX)

#VERSION = MAJOR.MINOR.PATCH
set(VERSIONING_MAJOR 0)#incompatible update
set(VERSIONING_MINOR 1)#new features added
set(VERSIONING_PATCH 0)#bug fixed / minor change


set(EXE_NAME "SOTCooker")


project(SOTCooker VERSION ${VERSIONING_MAJOR}.${VERSIONING_MINOR}.${VERSIONING_PATCH} LANGUAGES CXX)




###############################
#                             #
#        CMake config         #
#                             #
###############################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" "${CMAKE_ROOT}/Modules")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# --- CMake files includes ---
include(cmake/modules/Utils.cmake)


###############################
#                             #
#            LIBS             #
#                             #
###############################


if(NOT DEFINED BASE_PROTO_PATH)
    set(BASE_PROTO_PATH "C:\\msys64\\mingw64")
endif()


# --- Protobuf ---
#See README for more info about installation
set(Protobuf_PREFIX_PATH
    "${BASE_PROTO_PATH}\\include"            
    "${BASE_PROTO_PATH}\\lib"             
    "${BASE_PROTO_PATH}\\bin" )
list(APPEND CMAKE_PREFIX_PATH "${Protobuf_PREFIX_PATH}")

set(Protobuf_USE_STATIC_LIBS ON)
find_package(Protobuf REQUIRED HINTS "${BASE_PROTO_PATH}\\lib\\cmake\\protobuf")

message(STATUS "Protobuf include dir: ${Protobuf_INCLUDE_DIRS}")

if(NOT DEFINED Protobuf_INCLUDE_DIRS)
    set(Protobuf_INCLUDE_DIRS "${BASE_PROTO_PATH}\\include" CACHE BOOL "")
endif()
include_directories(SYSTEM ${Protobuf_INCLUDE_DIRS})

find_package(Protobuf)
include_directories(SYSTEM ${Protobuf_INCLUDE_DIRS})

# --- Qt ---
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)


# --- Sudirectories ---

add_subdirectory(Libs)




###############################
#                             #
#          Sources            #
#                             #
###############################

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/Assets/iconResource.rc")

set(PROJECT_SOURCES
        LoggerHandler.hpp

        Constants.hpp
        
        main.cpp
        MainWindow.cpp
        MainWindow.hpp
        MainWindow.ui

        GraphicsUtils.hpp
        CircularProgressBar.hpp

        ProgResources.qrc
)



###############################
#                             #
#        Compilation          #
#                             #
###############################

add_compile_options(-Wall -Wextra -Wpedantic)

qt_add_executable(${EXE_NAME} WIN32
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${APP_ICON_RESOURCE_WINDOWS}
)

target_link_libraries(${EXE_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    libSimpleUpdater::libSimpleUpdater
    CookerProto::CookerProto
    SotTools::SotTools
    QtWindowsUtils::QtWindowsUtils
    TextToSpeech::TextToSpeech
    ${PROTOBUF_LIBRARIES}
)


qt_finalize_executable(${EXE_NAME})

# --- Properties ---

set_property(GLOBAL PROPERTY COMPILE_WARNING_AS_ERROR OFF)


###############################
#                             #
# Code macros (definitions)   #
#                             #
###############################

add_definitions("-DPROJECT_V_MAJOR=${VERSIONING_MAJOR}")
add_definitions("-DPROJECT_V_MINOR=${VERSIONING_MINOR}")
add_definitions("-DPROJECT_V_PATCH=${VERSIONING_PATCH}")
add_definitions("-DPROJECT_GITHUB_RELEASE_DEFINE=\"https://api.github.com/repos/mathieu0c/SOTCooker/releases/latest\"")

add_definitions("-DPROJECT_VERSION=\"${CMAKE_PROJECT_VERSION}\"")
add_definitions("-DPROJECT_NAME=\"${CMAKE_PROJECT_NAME}\"")
add_definitions("-DWPROJECT_NAME=L\"${CMAKE_PROJECT_NAME}\"")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(CMAKE_DEBUG_MODE)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

add_definitions("-DCMAKE_PROGRAM_NAME=\"${EXE_NAME}\"")

#define LOG_HANDLER_BASE_DIR_QSTRING
IF(CMAKE_BUILD_TYPE MATCHES Debug)
    set(LOG_HANDLER_BASE_DIR_MACRO "-DLOG_HANDLER_BASE_DIR_QSTRING=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+\"/\"+QString{\"${EXE_NAME}_debug\"}" CACHE BOOL "" FORCE)
ELSE()
    set(LOG_HANDLER_BASE_DIR_MACRO "-DLOG_HANDLER_BASE_DIR_QSTRING=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+\"/\"+QString{\"${EXE_NAME}\"}" CACHE BOOL "" FORCE)
ENDIF()

add_definitions(${LOG_HANDLER_BASE_DIR_MACRO})
