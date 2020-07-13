find_package(Threads)

add_executable(server src/communication.cc src/dynamic_loading.cc src/execution.cc src/cli.cc src/error.cc src/main.cc)
target_include_directories(server PRIVATE include/)
target_link_libraries(server PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)
target_compile_definitions(server PRIVATE SERVER_SOCKET_PATH="${CMAKE_BINARY_DIR}/server_socket")
