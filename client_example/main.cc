#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <map_reduce_api.hpp>

using namespace map_reduce;

int main(int argc, const char **argv)
{
  if (argc != 3)
  {
    std::cerr << "Expecting exactly two arguments - task name and number of threads.\n";
    return 1;
  }

  if (strlen(argv[1]) > CLIENT_NAME_MAX_LEN)
  {
    std::cerr << "Client name too long.\n";
    return 1;
  }

  std::string lib_path = std::string(SHARED_LIBS_PATH) + "/lib" + argv[1] + ".so";

  if (lib_path.length() > LIB_PATH_MAX_LEN)
  {
    std::cerr << "Library path too long.\n";
    return 1;
  }

  Connection conn;
  ClientRequest request;
  ErrorCode err;
  ErrorCode server_reply;

  strcpy(request.client_name, argv[1]);
  strcpy(request.lib_path, lib_path.c_str());
  request.num_threads = atoi(argv[2]);


  err = conn.send_request(request);

  if (err != ErrorCode::OK)
  {
    logger(err);
    return 1;
  }

  err = conn.get_reply(server_reply);

  if (err != ErrorCode::OK)
  {
    logger(err);
    return 1;
  }

  logger(server_reply);

  return 0;
}
