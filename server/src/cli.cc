#include <thread>
#include <poll.h>
#include <iostream>
#include <cstring>

#include "cli.h"

namespace map_reduce
{

CommandLineInterface::CommandLineInterface(Master &master, Connection &conn, std::atomic_bool &quit)
: master(master),
  conn(conn),
  quit_main(quit),
  execution_thread(&CommandLineInterface::cli_routine, this),
  stop(false)
{
}

CommandLineInterface::~CommandLineInterface()
{
  stop.store(true);
  execution_thread.join();
}

#define BUFFER_SIZE 16

void CommandLineInterface::cli_routine()
{
  char command_buffer[BUFFER_SIZE];
  pollfd fds;
  fds.fd = 0;
  fds.events = POLLIN;

  while(!stop.load())
  {
    int ret = poll(&fds, 1, 0);

    if (ret == 1 && fds.revents == POLLIN)
    {
      std::cin.getline(command_buffer, BUFFER_SIZE);
      process_command(command_buffer);
    }
  }
}

void CommandLineInterface::process_command(const char *cmd)
{
  if (strcmp(cmd, "quit") == 0)
  {
    quit_main.store(true);
    conn.interrupt();
    stop.store(true);
  }
  else if (strcmp(cmd, "task stop") == 0)
  {
    master.interrupt();
  }
  else if (strcmp(cmd, "task info") == 0)
  {
    master.print_task_info();
  }
}

}
