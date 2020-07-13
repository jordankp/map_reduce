#pragma once

#include "error.h"

namespace map_reduce
{

struct ClientFunctions
{
  void *read = nullptr;
  void *map = nullptr;
  void *reduce = nullptr;
  void *write = nullptr;
};

ErrorCode load_client_library(const char *path, ClientFunctions &funcs);

}
