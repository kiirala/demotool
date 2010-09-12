#include <gtkmm.h>
#include <gtk/gtkgl.h>

#include "datastorage.h"
#include "moduleloader.h"
#include "gui.h"

int main(int argc, char **argv) {
  Gtk::Main kit(argc, argv);
  gtk_gl_init(&argc, &argv);

  DataStorage ds("default.xml");
  ModuleLoader ml(ds);

  Gui gui(ml);

  gui.run();

  return 0;
}
