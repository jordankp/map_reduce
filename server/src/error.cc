#include <iostream>

#include "error.h"

namespace map_reduce
{

void logger(ErrorCode err)
{
  switch (err)
  {
    case ErrorCode::OK:
      std::cerr << "[INFO] Task finished.\n";
      break;
    case ErrorCode::SOCKET_SERVER_CREATE_FAIL:
      std::cerr << "[ERROR] Failed to create server socket.\n";
      break;
    case ErrorCode::SOCKET_CLIENT_CREATE_FAIL:
      std::cerr << "[ERROR] Failed to create client socket.\n";
      break;
    case ErrorCode::SOCKET_BIND_FAIL:
      std::cerr << "[ERROR] Bind operation on server socket failed.\n";
      break;
    case ErrorCode::SOCKET_LISTEN_FAIL:
      std::cerr << "[ERROR] Listen operation on server socket failed.\n";
      break;
    case ErrorCode::SOCKET_ACCEPT_FAIL:
      std::cerr << "[ERROR] Accept operation on server socket failed.\n";
      break;
    case ErrorCode::SOCKET_READ_SERVER_FAIL:
      std::cerr << "[ERROR] Failed to read from client.\n";
      break;
    case ErrorCode::SOCKET_READ_CLIENT_FAIL:
      std::cerr << "[ERROR] Failed to read from server.\n";
      break;
    case ErrorCode::SOCKET_WRITE_SERVER_FAIL:
      std::cerr << "[ERROR] Failed to write to client.\n";
      break;
    case ErrorCode::SOCKET_WRITE_CLIENT_FAIL:
      std::cerr << "[ERROR] Failed to write to server.\n";
      break;
    case ErrorCode::SOCKET_CONNECT_FAIL:
      std::cerr << "[ERROR] Connect operation on client socket failed.\n";
      break;
    case ErrorCode::DLOPEN_FAIL:
      std::cerr << "[ERROR] Failed to open client shared library.\n";
      break;
    case ErrorCode::DLSYM_FAIL:
      std::cerr << "[ERROR] Failed to find a function in client shared library.\n";
      break;
    case ErrorCode::TASK_INTERRUPTED:
      std::cerr << "[WARNING] Client task was interrupted.\n";
      break;
  }
}

}
