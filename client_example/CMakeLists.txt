add_executable(client_example main.cc)
target_link_libraries(client_example PRIVATE map_reduce_api)
target_compile_definitions(client_example PRIVATE SHARED_LIBS_PATH="${CMAKE_BINARY_DIR}/lib")

add_library(word_count SHARED word_count/functions.cc)
