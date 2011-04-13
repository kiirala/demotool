#ifndef DEMOTOOL_GUI_H
#define DEMOTOOL_GUI_H

class ModuleLoader;
class Window;

class Gui {
public:
  Gui(ModuleLoader &ml);

  void run();

private:
  ModuleLoader &loader;

  void on_reload();
  bool on_build();
  void on_resize();
  void on_redraw();
  void load_libs();

  Window &win;
  void *renderer_lib;
  void (*render)(double time);
  void (*resize)(int width, int height);
};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_GUI_H */
