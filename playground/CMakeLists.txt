aux_source_directory(. PLAYGROUND_SOURCES)
add_executable(playground ${PLAYGROUND_SOURCES})
target_link_libraries(playground formal)
