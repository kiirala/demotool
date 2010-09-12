#include <stdio.h>
#include <dlfcn.h>

#include "moduleloader.h"
#include "datastorage.h"

ModuleLoader::ModuleLoader(DataStorage &ds)
  : data(ds)
{

}

ModuleLoader::~ModuleLoader() {

}

void* ModuleLoader::load(const char* filename) {
  void* lib = dlopen(filename, RTLD_LAZY | RTLD_LOCAL);
  if (!lib) {
    fprintf(stderr, "Error loading %s: %s\n", filename, dlerror());
  }
  return lib;
}

void* ModuleLoader::get(void *lib, const char* name) {
  void* sym = dlsym(lib, name);
  if (!sym) {
    fprintf(stderr, "Symbol %s not found: %s\n", name, dlerror());
  }
  return sym;
}

void ModuleLoader::unload(void* lib) {
  int ret = dlclose(lib);
  if (ret != 0) {
    fprintf(stderr, "Close failed: %s\n", dlerror());
  }
}
