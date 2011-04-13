#ifndef DEMOTOOL_MODULELOADER_H
#define DEMOTOOL_MODULELOADER_H

class ModuleLoader {
public:
  ModuleLoader();
  ~ModuleLoader();

  void* load(const char* filename);
  void* get(void* lib, const char* name);
  void unload(void* lib);

private:

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_MODULELOADER_H */
