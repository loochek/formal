aux_source_directory(. CYK_SOURCES)
add_executable(cyk ${CYK_SOURCES})
target_link_libraries(cyk formal)
