include_directories (${Boost_INCLUDE_DIRS})
include_directories (${SOCI_INCLUDE} ${SOCI_SQLITE3_INCLUDE})

add_executable (slin main.cpp link.cpp database.cpp utils.cpp)

target_link_libraries (slin ${SOCI_LIBRARY} ${SOCI_SQLITE3_LIBRARY} ${USTL_LIBRARY} ${UTIO_LIBRARY} ${YAML_CPP_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} "dl")
