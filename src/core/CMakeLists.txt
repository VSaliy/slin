set (LIBSLIN_SOURCE link.cpp database.cpp utils.cpp configfile.cpp)
set (LIBSLIN_HEADERS link.hpp database.hpp utils.hpp configfile.hpp)

add_library (slin-core ${LIBSLIN_SOURCE})
install (FILES ${LIBSLIN_HEADERS} DESTINATION include/slin-core)
install (TARGETS slin-core
         RUNTIME DESTINATION bin
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib)

target_link_libraries (slin-core ${SOCI_LIBRARY} ${SOCI_SQLITE3_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${LIBCURL_LIBRARY})

# Soci needs libdl on Unix
if (UNIX)
    target_link_libraries (slin-core "dl")
endif (UNIX)
