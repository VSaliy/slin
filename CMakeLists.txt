cmake_minimum_required (VERSION 2.8)
project (slin)

include (CheckCXXCompilerFlag)

macro (ADD_CXX_FLAG_IF_SUPPORTED FLAGNAME FLAG)
    CHECK_CXX_COMPILER_FLAG (${FLAG} ${FLAGNAME})
    if (${FLAGNAME})
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
    endif (${FLAGNAME})
endmacro()

ADD_CXX_FLAG_IF_SUPPORTED ("CXX11" "-std=c++0x")
ADD_CXX_FLAG_IF_SUPPORTED ("Wall" "-Wall")
ADD_CXX_FLAG_IF_SUPPORTED ("Wextra" "-Wextra")

option (BUILD_SHARED_LIBS "Build shared libraries." OFF)

include (CPack)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "A small link manager for the cli")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "1")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_PACKAGE_EXECUTABLES "slin")

find_package (Curses)

if(CURSES_FOUND OR WIN32)
	option(WITH_COLOR "Enables color output" ON)
else(CURSES_FOUND OR WIN32)
	option(WITH_COLOR "Enables color output" OFF)
endif(CURSES_FOUND OR WIN32)

find_library (LIBCURL_LIBRARY NAMES libcurldll curl libcurl)

find_path (SOCI_INCLUDE soci.h PATH_SUFFIXES soci)
find_path (SOCI_SQLITE3_INCLUDE soci-sqlite3.h PATHS "${SOCI_INCLUDE}/sqlite3/")
find_library (SOCI_LIBRARY soci_core libsoci_core)
find_library (SOCI_SQLITE3_LIBRARY soci_sqlite3 libsoci_sqlite3)

find_package (Threads)
find_package (Boost 1.46.0 REQUIRED COMPONENTS system filesystem)

add_subdirectory (src)
