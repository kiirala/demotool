#include <stdio.h>
#include <dlfcn.h>

#include "moduleloader.h"

void* ModuleLoader::load(const char* filename) {
  void* lib = dlopen(filename, RTLD_LAZY | RTLD_LOCAL);
  if (!lib) {
    fprintf(stderr, "Error loading %s: %s\n", filename, dlerror());
  }
  void (*load_f)() = (void(*)())get(lib, "load");
  if (load_f) load_f();
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
  void (*unload_f)() = (void(*)())get(lib, "unload");
  if (unload_f) unload_f();

  int ret = dlclose(lib);
  if (ret != 0) {
    fprintf(stderr, "Close failed: %s\n", dlerror());
  }
}
