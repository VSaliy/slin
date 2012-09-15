cmake_minimum_required (VERSION 2.8)
project (slin)

include (CPack)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "A small link manager for the cli")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "1")
set (CPACK_PACKAGE_VERSION_PATCH "0")
set (CPACK_PACKAGE_EXECUTABLES "slin")

set (CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++0x")

find_library (USTL_LIBRARY NAMES ustl libustl)
find_library (UTIO_LIBRARY NAMES utio libutio)

if(USTL_LIBRARY AND UTIO_LIBRARY)
	option(WITH_COLOR "Enables color output" ON)
else(USTL_LIBRARY AND UTIO_LIBRARY)
	option(WITH_COLOR "Enables color output" OFF)
endif(USTL_LIBRARY AND UTIO_LIBRARY)

find_library (YAML_CPP_LIBRARY NAMES yaml-cpp libyaml-cpp)
find_path (YAML_CPP_INCLUDE yaml-cpp/yaml.h)
find_library (LIBCURL_LIBRARY NAMES libcurldll curl libcurl)

find_path (SOCI_INCLUDE soci.h PATH_SUFFIXES soci)
find_path (SOCI_SQLITE3_INCLUDE soci-sqlite3.h PATHS "${SOCI_INCLUDE}/sqlite3/")
find_library (SOCI_LIBRARY soci_core libsoci_core)
find_library (SOCI_SQLITE3_LIBRARY soci_sqlite3 libsoci_sqlite3)

find_package (Threads)
find_package (Boost 1.46.0 REQUIRED COMPONENTS system filesystem)

add_subdirectory (src)
