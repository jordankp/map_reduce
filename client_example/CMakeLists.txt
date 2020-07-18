add_executable(client_example main.cc)
target_link_libraries(client_example PRIVATE map_reduce_api)
target_compile_definitions(client_example PRIVATE SHARED_LIBS_PATH="${CMAKE_BINARY_DIR}/lib")

add_library(word_count SHARED word_count/functions.cc)
target_compile_definitions(word_count PRIVATE INPUT_DIR="${CMAKE_SOURCE_DIR}/client_example/input_data"
                                              OUTPUT_DIR="${CMAKE_SOURCE_DIR}/client_example/word_count")

add_library(inverted_index SHARED inverted_index/functions.cc)
target_compile_definitions(inverted_index PRIVATE INPUT_DIR="${CMAKE_SOURCE_DIR}/client_example/input_data"
                                              OUTPUT_DIR="${CMAKE_SOURCE_DIR}/client_example/inverted_index")
