cmake_minimum_required (VERSION 2.8)
project (slin)

set (CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++11")

find_library (USTL_LIBRARY NAMES ustl libustl)
find_library (UTIO_LIBRARY NAMES utio libutio)
find_library (YAML_CPP_LIBRARY NAMES yaml-cpp libyaml-cpp)

find_package (Threads)
find_package (Boost 1.46.0 REQUIRED COMPONENTS system filesystem)

add_subdirectory (sqlite)
add_subdirectory (src)
