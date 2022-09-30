aux_source_directory(src FORMAL_SOURCES)
add_library(formal STATIC ${FORMAL_SOURCES})
target_link_libraries(formal fmt::fmt)
target_include_directories(formal PUBLIC include/)
