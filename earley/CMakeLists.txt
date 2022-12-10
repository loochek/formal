aux_source_directory(. EARLEY_SOURCES)
add_executable(earley ${EARLEY_SOURCES})
target_link_libraries(earley formal)
