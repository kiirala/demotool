#include <gtkmm.h>
#include <gtk/gtkgl.h>

#include "moduleloader.h"
#include "gui.h"

int main(int argc, char **argv) {
  Gtk::Main kit(argc, argv);
  gtk_gl_init(&argc, &argv);

  ModuleLoader ml;

  Gui gui(ml);

  gui.run();

  return 0;
}
