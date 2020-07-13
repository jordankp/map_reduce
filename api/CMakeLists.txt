add_library(map_reduce_api INTERFACE)
target_include_directories(map_reduce_api INTERFACE .)
target_compile_definitions(map_reduce_api INTERFACE SERVER_SOCKET_PATH="${CMAKE_BINARY_DIR}/server_socket")
