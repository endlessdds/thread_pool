set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)

include_directories(${PROJECT_SOURCE_DIR}/include)

link_libraries(/usr/local/lib/libgtest.a)

add_executable(test ${SRC_LIST})

find_package(Threads)
target_link_libraries(test ${CMAKE_THREAD_LIBS_INIT})
