#ifndef DEMOTOOL_GUI_H
#define DEMOTOOL_GUI_H

class ModuleLoader;
class Window;

class Gui {
public:
  Gui(ModuleLoader &ml);

  void run();

private:
  void on_reload();
  void on_resize();
  void on_redraw();
  void load_libs();

  ModuleLoader &loader;
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
