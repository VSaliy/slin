if (WITH_COLOR AND CURSES_FOUND)
    include_directories (SYSTEM ${CURSES_INCLUDE_DIR})
endif ()

set (SLIN_SOURCE main.cpp)

# Platform specific coloroutput files
if (WITH_COLOR AND CURSES_FOUND)
	list (APPEND SLIN_SOURCE coloroutput_curses.cpp)
elseif (WITH_COLOR AND WIN32)
	list (APPEND SLIN_SOURCE coloroutput_win32.cpp)
else ()
	list (APPEND SLIN_SOURCE coloroutput_generic.cpp) # No colors :(
endif ()

add_executable (slin ${SLIN_SOURCE})

install (TARGETS slin
         RUNTIME DESTINATION bin)

configure_file (${CMAKE_SOURCE_DIR}/src/cli/config.hpp.cmake
    ${CMAKE_SOURCE_DIR}/src/cli/config.hpp)

if (WITH_COLOR AND CURSES_FOUND)
	target_link_libraries (slin ${CURSES_LIBRARIES})
endif (WITH_COLOR AND CURSES_FOUND)

target_link_libraries (slin slin-core ${SOCI_LIBRARY} ${SOCI_SQLITE3_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${LIBCURL_LIBRARY})
