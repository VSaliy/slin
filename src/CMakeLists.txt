include_directories (${PROJECT_SOURCE_DIR}/sqlite/)
include_directories (${Boost_INCLUDE_DIRS})

add_executable (slin main.cpp link.cpp database.cpp utils.cpp)

target_link_libraries (slin sqlite ${USTL_LIBRARY} ${UTIO_LIBRARY} ${YAML_CPP_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES} "dl")
