#pragma once

#include <atomic>

#include "execution.h"
#include "communication.h"

namespace map_reduce
{

class CommandLineInterface
{
  Master &master;
  Connection &conn;
  std::atomic_bool &quit_main;

  std::atomic_bool stop;
  std::thread execution_thread;

public:

  CommandLineInterface(Master &master, Connection &conn, std::atomic_bool &quit);
  ~CommandLineInterface();

private:

  void cli_routine();
  void process_command(const char *cmd);
};

}
