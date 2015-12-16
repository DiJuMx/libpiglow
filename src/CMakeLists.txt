set(CMAKE_C_FLAGS "-Wall -pedantic")

add_library(piglow SHARED libpiglow.c)

install(TARGETS piglow 
    LIBRARY DESTINATION lib)
