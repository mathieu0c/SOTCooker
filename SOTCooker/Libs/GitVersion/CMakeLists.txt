cmake_minimum_required (VERSION 3.8)

set(LIB_NAME GitVersion)

message("------- <${LIB_NAME}> -------")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CUSTOM_ENABLE_TESTING OFF)

project(${LIB_NAME})

set(hack_file hack_file)
set(output_cpp git_version_generated.cpp)

include(FindPythonInterp)

add_custom_command(
  OUTPUT ${output_cpp}
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_cpp.py ${CMAKE_CURRENT_SOURCE_DIR}/git_version_template.txt ${output_cpp}
  COMMAND ${CMAKE_COMMAND} -E remove ${hack_file}
  DEPENDS ${hack_file}
)

add_custom_command(OUTPUT ${hack_file}
COMMAND ${CMAKE_COMMAND} -E touch ${hack_file})

set(SOURCE_FILES
    git_version.hpp
#    git_version_generated.cpp
    git_version_structs.hpp
)

# We declare the project as a static library and add all the source code files to it.
add_library(${LIB_NAME} STATIC ${SOURCE_FILES} ${output_cpp})
#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(libGitVersion::libGitVersion ALIAS ${LIB_NAME})

# add_dependencies(${LIB_NAME} generate_cpp)
#add_custom_command(
#  TARGET ${LIB_NAME} PRE_BUILD
#  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generate_cpp.py ${CMAKE_CURRENT_SOURCE_DIR}/git_version_template.txt ${CMAKE_CURRENT_SOURCE_DIR}/git_version_generated.cpp
#  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)




#Set target properties
target_include_directories(${LIB_NAME}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
   PRIVATE
       ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(${LIB_NAME} PRIVATE cxx_auto_type)
target_compile_options(${LIB_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall -Wextra -Wpedantic>)

#install(
#    FILES ${CMAKE_CURRENT_BINARY_DIR}/${output_cpp} ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp
#    DESTINATION version_cpp
#)

if(CUSTOM_ENABLE_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

message("----- END <${LIB_NAME}> -----")
