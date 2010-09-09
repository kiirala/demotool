#ifndef DEMOTOOL_MODULELOADER_H
#define DEMOTOOL_MODULELOADER_H

class DataStorage;

class ModuleLoader {
public:
  ModuleLoader(DataStorage &ds);
  ~ModuleLoader();

private:
  DataStorage &data;

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_MODULELOADER_H */
