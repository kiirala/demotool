#include <gtkmm.h>
#include <gtk/gtkgl.h>

#include "datastorage.h"
#include "moduleloader.h"
#include "scheduler.h"
#include "gui.h"

int main(int argc, char **argv) {
  Gtk::Main kit(argc, argv);
  gtk_gl_init(&argc, &argv);

  DataStorage ds("default.xml");
  ModuleLoader ml(ds);
  Scheduler sch(ds, ml);

  Gui gui(ml, sch);

  gui.run();

  return 0;
}
