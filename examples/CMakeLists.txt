# examples/CMakeLists.txt

# Standard I/O examples
add_executable(stdio_client calculator_example/stdio/client.cpp)
target_link_libraries(stdio_client PRIVATE jsonrpc)

add_executable(stdio_server calculator_example/stdio/server.cpp)
target_link_libraries(stdio_server PRIVATE jsonrpc)

# Unix domain socket examples
add_executable(pipe_client calculator_example/pipe/client.cpp)
target_link_libraries(pipe_client PRIVATE jsonrpc)

add_executable(pipe_server calculator_example/pipe/server.cpp)
target_link_libraries(pipe_server PRIVATE jsonrpc)

# Framed Unix domain socket examples
add_executable(framed_pipe_client calculator_example/framed_pipe/client.cpp)
target_link_libraries(framed_pipe_client PRIVATE jsonrpc)

add_executable(framed_pipe_server calculator_example/framed_pipe/server.cpp)
target_link_libraries(framed_pipe_server PRIVATE jsonrpc)

# Socket examples
add_executable(socket_client calculator_example/socket/client.cpp)
target_link_libraries(socket_client PRIVATE jsonrpc)

add_executable(socket_server calculator_example/socket/server.cpp)
target_link_libraries(socket_server PRIVATE jsonrpc)

# Framed socket examples
add_executable(framed_socket_client calculator_example/framed_socket/client.cpp)
target_link_libraries(framed_socket_client PRIVATE jsonrpc)

add_executable(framed_socket_server calculator_example/framed_socket/server.cpp)
target_link_libraries(framed_socket_server PRIVATE jsonrpc)

# LSP example
add_executable(pipe_lsp_server lsp_example/pipe_lsp_server.cpp)
target_link_libraries(pipe_lsp_server PRIVATE jsonrpc)
