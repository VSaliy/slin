set (LIBSLIN_SOURCE link.cpp database.cpp utils.cpp configfile.cpp)

add_library (slin-core ${LIBSLIN_SOURCE})
install (TARGETS slin-core DESTINATION lib)

target_link_libraries (slin-core ${SOCI_LIBRARY} ${SOCI_SQLITE3_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} ${LIBCURL_LIBRARY})

# Soci needs libdl on Unix
if (UNIX)
    target_link_libraries (slin-core "dl")
endif (UNIX)
