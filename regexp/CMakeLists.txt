aux_source_directory(. REGEXP_SOURCES)
add_executable(regexp ${REGEXP_SOURCES})
target_link_libraries(regexp formal)
