#include <gtkmm.h>
#include <gtk/gtkgl.h>
#include <gdk/gdkgl.h>
#include <GL/gl.h>
#include <sys/time.h>

#include "gui.h"
#include "moduleloader.h"

bool running;
bool play;

class Window : public Gtk::Window {
public:
  ModuleLoader loader;

  Gtk::VPaned vsplit;
  Gtk::HPaned top;
  Gtk::EventBox draw_area;
  Gtk::Frame dummy1;
  Gtk::VBox toolbox;
  Gtk::Adjustment time;
  Gtk::HScale time_w;
  Gtk::HBox toolbox_buttons;
  Gtk::Button next_frame, prev_frame, play_b, reload, rebuild;

  sigc::signal<void> signal_reload, signal_resize, signal_redraw;
  sigc::signal<bool> signal_build;

  void on_quit() {
    running = false;
  }

  void on_draw_resize(Gdk::Rectangle &/*ev*/) {
    signal_resize.emit();
  }

  void on_time_changed() {
    if (!play)
      signal_redraw.emit();
  }

  void on_prev_frame() {
    time.set_value(time.get_value() - 1.0/60);
    time_w.queue_draw();
    signal_redraw.emit();
  }

  void on_next_frame() {
    time.set_value(time.get_value() + 1.0/60);
    time_w.queue_draw();
    signal_redraw.emit();
  }

  void on_reload() {
    signal_reload.emit();
  }

  void on_rebuild() {
    if (signal_build.emit())
      signal_reload.emit();
  }

  void on_ready() {
    printf("Ready!\n");
  }

  double clock_play_start;
  double slider_play_start;

  void on_play() {
    if (play) {
      play = false;
      play_b.set_label("Play");
    }
    else {
      timeval now;
      slider_play_start = time.get_value();
      gettimeofday(&now, 0);
      clock_play_start = now.tv_sec + now.tv_usec / 1000000.0;
      play = true;
      play_b.set_label("Pause");
    }
  }

  void advance_time() {
    timeval now;
    gettimeofday(&now, 0);
    double t_now = now.tv_sec + now.tv_usec / 1000000.0;
    time.set_value(slider_play_start + (t_now - clock_play_start));
    time_w.queue_draw();
    if (time.get_value() == time.get_upper()) {
      play = false;
      play_b.set_label("Play");
    }
  }

  Window(ModuleLoader &ml)
    : loader(ml), time(0.0, 0.0, 120.0, 1.0/60, 1.0, 0.0), time_w(time),
      next_frame(">"), prev_frame("<"), play_b("Play"),
      reload("Reload"), rebuild("Rebuild")
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

    time.signal_value_changed().connect(sigc::mem_fun(*this, &Window::on_time_changed));
    time_w.set_size_request(200, 50);
    time_w.set_digits(2);
    time_w.show();
    toolbox.pack_start(time_w, false, true);

    prev_frame.signal_clicked().connect(sigc::mem_fun(*this, &Window::on_prev_frame));
    prev_frame.show();
    toolbox_buttons.pack_start(prev_frame, false, true);

    next_frame.signal_clicked().connect(sigc::mem_fun(*this, &Window::on_next_frame));
    next_frame.show();
    toolbox_buttons.pack_start(next_frame, false, true);

    play_b.signal_clicked().connect(sigc::mem_fun(*this, &Window::on_play));
    play_b.show();
    toolbox_buttons.pack_start(play_b, false, true);

    reload.signal_clicked().connect(sigc::mem_fun(*this, &Window::on_reload));
    reload.show();
    toolbox_buttons.pack_end(reload, false, true);

    rebuild.signal_clicked().connect(sigc::mem_fun(*this, &Window::on_rebuild));
    rebuild.show();
    toolbox_buttons.pack_end(rebuild, false, true);

    toolbox_buttons.show();
    toolbox.pack_end(toolbox_buttons, true, true);
    toolbox.show();

    vsplit.pack1(top, true, true);
    vsplit.pack2(toolbox, false, true);

    add(vsplit);

    top.show();
    vsplit.show();
    show();
    draw_area.signal_size_allocate().connect_notify(sigc::mem_fun(*this, &Window::on_draw_resize));
    //draw_area.signal_show().connect(sigc::mem_fun(*this, &Window::on_ready));
  }

  GdkGLContext *gl_context() {
    return gtk_widget_get_gl_context((GtkWidget*)draw_area.gobj());
  }

  GdkGLDrawable *gl_drawable() {
    return gtk_widget_get_gl_drawable((GtkWidget*)draw_area.gobj());
  }

};

Gui::Gui(ModuleLoader &ml)
  : loader(ml), win(*(new Window(ml))), render(0), resize(0)
{
  win.signal_reload.connect(sigc::mem_fun(*this, &Gui::on_reload));
  win.signal_build.connect(sigc::mem_fun(*this, &Gui::on_build));
  win.signal_resize.connect(sigc::mem_fun(*this, &Gui::on_resize));
  win.signal_redraw.connect(sigc::mem_fun(*this, &Gui::on_redraw));
  on_resize();
  running = true;
  play = false;
}

bool Gui::on_build() {
  int status = system("make");
  if (status < 0 || WEXITSTATUS(status) != 0)
    return false;
  return true;
}

void Gui::on_reload() {
  loader.unload(renderer_lib);
  resize = 0;
  render = 0;
  load_libs();
}

void Gui::on_resize() {
  if (!gdk_gl_drawable_gl_begin(win.gl_drawable(), win.gl_context()))
    g_assert_not_reached ();

  if (resize)
    resize(win.draw_area.get_width(), win.draw_area.get_height());

  gdk_gl_drawable_gl_end(win.gl_drawable());
  on_redraw();
}

void Gui::on_redraw() {
  GdkGLContext *gl_context = win.gl_context();
  GdkGLDrawable *gl_drawable = win.gl_drawable();

  if (!render)
    load_libs();

  if (!gdk_gl_drawable_gl_begin(gl_drawable, gl_context))
    g_assert_not_reached ();

  if (play)
    win.advance_time();
  render(win.time.get_value());

  if (gdk_gl_drawable_is_double_buffered(gl_drawable))
    gdk_gl_drawable_swap_buffers(gl_drawable);
  else
    glFlush();

  gdk_gl_drawable_gl_end(gl_drawable);
}

void Gui::load_libs() {
  void* renderer = loader.load("./renderer.so");
  render = (void (*)(double))loader.get(renderer, "render");
  resize = (void (*)(int, int))loader.get(renderer, "resize");
  renderer_lib = renderer;
  on_resize();
}

void Gui::run() {
  while(running) {
    on_redraw();
    //if (!play)
      Gtk::Main::iteration();
    while( Gtk::Main::events_pending() )
      Gtk::Main::iteration();
  }
}
