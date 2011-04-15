#include <cstdio>
#include <cstdlib>

#include <gtkmm.h>
#include <gtk/gtkgl.h>

#include "moduleloader.h"
#include "gui.h"

int main(int argc, char **argv) {
  Gtk::Main kit(argc, argv);
  gtk_gl_init(&argc, &argv);

  ModuleLoader ml;

  if (argc != 2) {
    fprintf(stderr,
	    "Usage: %s projectname\n",
	    argv[0]);
    exit(EXIT_FAILURE);
  }
  Gui gui(ml, argv[1]);

  gui.run();

  return 0;
}
