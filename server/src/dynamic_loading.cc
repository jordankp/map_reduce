#include <dlfcn.h>

#include "dynamic_loading.h"

namespace map_reduce
{

ErrorCode load_client_library(const char *path, ClientFunctions &funcs)
{
  static void *handle = nullptr;

  if (handle)
    dlclose(handle);

  handle = dlopen(path, RTLD_NOW);

  if (!handle)
    return ErrorCode::DLOPEN_FAIL;

  funcs.read = dlsym(handle, "read");
  if (dlerror())
    return ErrorCode::DLSYM_FAIL;

  funcs.map = dlsym(handle, "map");
  if (dlerror())
    return ErrorCode::DLSYM_FAIL;

  funcs.reduce = dlsym(handle, "reduce");
  if (dlerror())
    return ErrorCode::DLSYM_FAIL;

  funcs.write = dlsym(handle, "write");
  if (dlerror())
    return ErrorCode::DLSYM_FAIL;

  return ErrorCode::OK;
}

}
