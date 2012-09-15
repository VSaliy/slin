include_directories (${Boost_INCLUDE_DIRS})
include_directories (${YAML_CPP_INCLUDE} ${SOCI_INCLUDE} ${SOCI_SQLITE3_INCLUDE})

add_executable (slin main.cpp link.cpp database.cpp utils.cpp)
install (TARGETS slin
		 DESTINATION bin)

configure_file (${CMAKE_SOURCE_DIR}/src/config.hpp.cmake ${CMAKE_SOURCE_DIR}/src/config.hpp)

if(WITH_COLOR)
	target_link_libraries (slin ${USTL_LIBRARY} ${UTIO_LIBRARY} )
endif(WITH_COLOR)

target_link_libraries (slin ${SOCI_LIBRARY} ${SOCI_SQLITE3_LIBRARY} ${YAML_CPP_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${LIBCURL_LIBRARY})
if(UNIX)
	target_link_libraries (slin "dl")
endif(UNIX)
