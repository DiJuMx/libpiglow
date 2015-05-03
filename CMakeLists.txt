cmake_minimum_required( VERSION 2.6 )

project(piglow)

set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")

include_directories(include)
add_subdirectory(src)

install(FILES include/piglow.h 
    DESTINATION include)
