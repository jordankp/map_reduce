#pragma once

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

namespace map_reduce
{

enum class ErrorCode
{
  OK = 0,
  SOCKET_SERVER_CREATE_FAIL,
  SOCKET_CLIENT_CREATE_FAIL,
  SOCKET_BIND_FAIL,
  SOCKET_LISTEN_FAIL,
  SOCKET_ACCEPT_FAIL,
  SOCKET_READ_SERVER_FAIL,
  SOCKET_READ_CLIENT_FAIL,
  SOCKET_WRITE_SERVER_FAIL,
  SOCKET_WRITE_CLIENT_FAIL,
  SOCKET_CONNECT_FAIL,
  DLOPEN_FAIL,
  DLSYM_FAIL,
  TASK_INTERRUPTED
};

inline void logger(ErrorCode err)
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

#define CLIENT_NAME_MAX_LEN 32
#define LIB_PATH_MAX_LEN 128

struct ClientRequest
{
  char client_name[CLIENT_NAME_MAX_LEN + 1];
  char lib_path[LIB_PATH_MAX_LEN + 1];
  size_t num_threads;
};

class Connection
{
  int socket_descriptor = -1;

public:

  ErrorCode send_request(const ClientRequest &request)
  {
    sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, SERVER_SOCKET_PATH);

    socket_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);

    if (socket_descriptor == -1)
      return ErrorCode::SOCKET_CLIENT_CREATE_FAIL;

    if (connect(socket_descriptor, (const sockaddr *)&saddr, sizeof(saddr)) == -1)
      return ErrorCode::SOCKET_CONNECT_FAIL;

    if (write(socket_descriptor, &request, sizeof(request)) != sizeof(request))
      return ErrorCode::SOCKET_WRITE_CLIENT_FAIL;

    return ErrorCode::OK;
  }

  ErrorCode get_reply(ErrorCode &reply)
  {
    if (read(socket_descriptor, &reply, sizeof(reply)) != sizeof(reply))
      return ErrorCode::SOCKET_READ_CLIENT_FAIL;

    return ErrorCode::OK;
  }
};

}
