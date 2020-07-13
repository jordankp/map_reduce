#pragma once

#include <atomic>

#include "error.h"

namespace map_reduce
{

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
  int server_socket_descriptor = -1;
  int client_socket_descriptor = -1;
  const int MAX_WAITING_CLIENTS = 10;
  bool is_initialized = false;
  std::atomic_bool interrupted;

public:
  Connection() : interrupted(false) {}
  ~Connection();

  ErrorCode initialize();
  ErrorCode open_client_request(ClientRequest &request);
  ErrorCode reply_to_current_client(ErrorCode reply);
  void close_client_request();
  void interrupt();
};

}
