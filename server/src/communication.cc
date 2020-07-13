#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>

#include "communication.h"

namespace map_reduce
{

ErrorCode Connection::initialize()
{
  if (!is_initialized)
  {
    sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, SERVER_SOCKET_PATH);

    server_socket_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_socket_descriptor == -1)
      return ErrorCode::SOCKET_SERVER_CREATE_FAIL;

    if (bind(server_socket_descriptor, (const sockaddr *)&saddr, sizeof(saddr)))
      return ErrorCode::SOCKET_BIND_FAIL;

    if (listen(server_socket_descriptor, MAX_WAITING_CLIENTS))
      return ErrorCode::SOCKET_LISTEN_FAIL;

    is_initialized = true;
  }

  return ErrorCode::OK;
}

ErrorCode Connection::open_client_request(ClientRequest &request)
{
  pollfd fds;
  fds.fd = server_socket_descriptor;
  fds.events = POLLIN;

  while (interrupted.load() == false)
  {
    int ret = poll(&fds, 1, 0);

    if (ret == 1 && fds.revents == POLLIN)
    {
      client_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
      if (client_socket_descriptor == -1)
        return ErrorCode::SOCKET_ACCEPT_FAIL;

      if (read(client_socket_descriptor, (void *)&request, sizeof(request)) != sizeof(request))
        return ErrorCode::SOCKET_READ_SERVER_FAIL;

      return ErrorCode::OK;
    }
  }

  return ErrorCode::OK;
}

void Connection::interrupt()
{
  interrupted.store(true);
}

ErrorCode Connection::reply_to_current_client(ErrorCode reply)
{
  if (write(client_socket_descriptor, (void *)&reply, sizeof(reply)) != sizeof(reply))
    return ErrorCode::SOCKET_WRITE_SERVER_FAIL;

  return ErrorCode::OK;
}

void Connection::close_client_request()
{
  close(client_socket_descriptor);
}

Connection::~Connection()
{
  if (is_initialized)
  {
    close(server_socket_descriptor);
    unlink(SERVER_SOCKET_PATH);
  }
}

}
