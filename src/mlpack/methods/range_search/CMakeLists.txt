# Define the files we need to compile.
# Anything not in this list will not be compiled into MLPACK.
set(SOURCES
  range_search.hpp
  range_search_impl.hpp
  range_search_rules.hpp
  range_search_rules_impl.hpp
  range_search_stat.hpp
)

# Add directory name to sources.
set(DIR_SRCS)
foreach(file ${SOURCES})
  set(DIR_SRCS ${DIR_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${file})
endforeach()
# Append sources (with directory name) to list of all MLPACK sources (used at
# the parent scope).
set(MLPACK_SRCS ${MLPACK_SRCS} ${DIR_SRCS} PARENT_SCOPE)

add_executable(range_search
  range_search_main.cpp
)
target_link_libraries(range_search
  mlpack
)
install(TARGETS range_search RUNTIME DESTINATION bin)
