#ifndef DEMOTOOL_SCHEDULER_H
#define DEMOTOOL_SCHEDULER_H

class DataStorage;
class ModuleLoader;

class Scheduler {
public:
  Scheduler(DataStorage &ds, ModuleLoader &ml);

  void render();

private:
  DataStorage &data;
  ModuleLoader &loader;

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_SCHEDULER_H */
