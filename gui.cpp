#include <gtkmm.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkgl.h>
#include <GL/gl.h>

#include "gui.h"
#include "moduleloader.h"
#include "scheduler.h"

bool running;

class Window : public Gtk::Window {
public:
  ModuleLoader &loader;
  Scheduler &scheduler;

  Gtk::VPaned vsplit;
  Gtk::HPaned top;
  Gtk::EventBox draw_area;
  Gtk::Frame dummy1, dummy2;

  void on_quit() {
    running = false;
  }

  void on_draw_resize(Gdk::Rectangle &ev) {
    if (!gdk_gl_drawable_gl_begin(gl_drawable(), gl_context()))
      g_assert_not_reached ();

    glViewport (0, 0, draw_area.get_width(), draw_area.get_height());

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (-10,10,-10,10,-20050,10000);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glScalef (10., 10., 10.);

    gdk_gl_drawable_gl_end(gl_drawable());
  }

  Window(ModuleLoader &ml, Scheduler &s)
    : loader(ml), scheduler(s)
  {
    set_border_width(10);
    set_title("Demotool");
    signal_hide().connect(sigc::mem_fun(*this, &Window::on_quit));
    
    top.pack1(dummy1, true, true);
    dummy1.show();

    //int glattrib[] = {GDK_GL_ATTRIB_LIST_NONE, 0};
    //GdkGLConfig *glconf = gdk_gl_config_new_for_screen(Gdk::Screen::get_default()->gobj(), glattrib);
    GdkGLConfig *glconf = gdk_gl_config_new_by_mode_for_screen(Gdk::Screen::get_default()->gobj(), (GdkGLConfigMode)(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE | GDK_GL_MODE_DEPTH));
    gtk_widget_set_gl_capability((GtkWidget*)draw_area.gobj(), glconf, 0, TRUE, GDK_GL_RGBA_TYPE);
    draw_area.set_size_request(320, 200);
    top.pack2(draw_area, true, true);
    draw_area.show();

    vsplit.pack1(top, true, true);
    vsplit.pack2(dummy2, true, true);
    dummy2.show();

    add(vsplit);

    top.show();
    vsplit.show();
    show();
    draw_area.signal_size_allocate().connect_notify(sigc::mem_fun(*this, &Window::on_draw_resize));
  }

  GdkGLContext *gl_context() {
    return gtk_widget_get_gl_context((GtkWidget*)draw_area.gobj());
  }

  GdkGLDrawable *gl_drawable() {
    return gtk_widget_get_gl_drawable((GtkWidget*)draw_area.gobj());
  }

};

Gui::Gui(ModuleLoader &ml, Scheduler &s)
  : loader(ml), scheduler(s), win(*(new Window(ml, s)))
{
  running = true;
}

void Gui::run() {
  GdkGLContext *gl_context = win.gl_context();
  GdkGLDrawable *gl_drawable = win.gl_drawable();

  while(running) {
    if (!gdk_gl_drawable_gl_begin(gl_drawable, gl_context))
      g_assert_not_reached ();

    scheduler.render();

    if (gdk_gl_drawable_is_double_buffered(gl_drawable))
      gdk_gl_drawable_swap_buffers(gl_drawable);
    else
      glFlush ();

    gdk_gl_drawable_gl_end(gl_drawable);

    //Gtk::Main::iteration();
    while( Gtk::Main::events_pending() )
      Gtk::Main::iteration();
  }
}
