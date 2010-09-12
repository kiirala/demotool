#ifndef DEMOTOOL_MODULELOADER_H
#define DEMOTOOL_MODULELOADER_H

class DataStorage;

class ModuleLoader {
public:
  ModuleLoader(DataStorage &ds);
  ~ModuleLoader();

  void* load(const char* filename);
  void* get(void* lib, const char* name);
  void unload(void* lib);

private:
  DataStorage &data;

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_MODULELOADER_H */
