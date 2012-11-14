include_directories (SYSTEM ${Boost_INCLUDE_DIRS} ${SOCI_INCLUDE} ${SOCI_SQLITE3_INCLUDE})
include_directories (core)

add_subdirectory (core)
add_subdirectory (cli)
