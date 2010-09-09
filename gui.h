#ifndef DEMOTOOL_GUI_H
#define DEMOTOOL_GUI_H

class ModuleLoader;
class Scheduler;
class Window;

class Gui {
public:
  Gui(ModuleLoader &ml, Scheduler &s);

  void run();

private:
  ModuleLoader &loader;
  Scheduler &scheduler;
  Window &win;

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_GUI_H */
