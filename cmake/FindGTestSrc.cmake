SET(GOOGLETEST_VERSION "1.10.0")
set(GTEST_SEARCH_PATH
    ${GTEST_SOURCE_DIR}
    ${PROJECT_ROOT_PATH}/third-party/googletest/googletest
    ${PROJECT_ROOT_PATH}/third-party/)

find_path(GTEST_SOURCE_DIR
    NAMES CMakeLists.txt src/gtest_main.cc
    PATHS ${GTEST_SEARCH_PATH})


# Debian installs gtest include directory in /usr/include, thus need to look
# for include directory separately from source directory.
find_path(GTEST_INCLUDE_DIR
    NAMES gtest/gtest.h
    PATH_SUFFIXES include
    HINTS ${GTEST_SOURCE_DIR}
    PATHS ${GTEST_SEARCH_PATH})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTestSrc DEFAULT_MSG
    GTEST_SOURCE_DIR
    GTEST_INCLUDE_DIR)
