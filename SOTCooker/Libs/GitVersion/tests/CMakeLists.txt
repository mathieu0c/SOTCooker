include(FetchContent)


FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

# Give source files as parameters
function(internal_add_test test_name)
    message("Creating test [${test_name}]")
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name}
        PRIVATE
        GTest::GTest
        libGitVersion::libGitVersion
    )
    gtest_discover_tests(${test_name})
endfunction()

internal_add_test(versions_tests versions_tests.cpp)