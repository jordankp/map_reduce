#include <iostream>
#include <atomic>

#include "communication.h"
#include "dynamic_loading.h"
#include "execution.h"
#include "cli.h"

namespace map_reduce
{

int Main()
{
  std::atomic_bool quit(false);
  ErrorCode err = ErrorCode::OK;

  Connection conn;
  Master master;

  err = conn.initialize();
  if (err != ErrorCode::OK)
  {
    logger(err);
    return 1;
  }

  CommandLineInterface cli(master, conn, quit);

  while (quit.load() == false)
  {
    err = ErrorCode::OK;
    ClientRequest request;
    ClientFunctions funcs;

    err = conn.open_client_request(request);
    if (err != ErrorCode::OK)
    {
      logger(err);
      continue;
    }

    if (quit.load() == true)
      break;

    err = load_client_library((const char *)request.lib_path, funcs);
    if (err != ErrorCode::OK)
    {
      logger(err);

      err = conn.reply_to_current_client(err);
      if (err != ErrorCode::OK)
        logger(err);
      
      conn.close_client_request();
      continue;
    }

    err = master.execute_task(funcs, request.client_name, request.num_threads);

    err = conn.reply_to_current_client(err);
    if (err != ErrorCode::OK)
      logger(err);

    conn.close_client_request();
  }

  return 0;
}

}

int main()
{
    return map_reduce::Main();
}
