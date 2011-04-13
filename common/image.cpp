#include <assert.h>
//#include <string.h>
//#include <math.h>

//#include <signal.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "image.h"
#include "triangulate.h"

struct outline {
  double x, y;
  bool corner;
  struct outline *next;
  struct outline *prev;
};

static void remove(outline *pos) {
  pos->next->prev = pos->prev;
  pos->prev->next = pos->next;
  pos->prev = 0;
  pos->next = 0;
}

float const *current_colour;
int shape_depth;

static void remove_useless(outline *line) {
  outline *pos = line;
  outline *end = line;
  do {
    while (!pos->next->corner &&
	   abs(pos->x - pos->next->x) < 1e-2 &&
	   abs(pos->y - pos->next->y) < 1e-2) {
      remove(pos->next);
      end = pos;
    }
    pos = pos->next;
  } while (pos != end);

  do {
    while (!pos->next->corner &&
	   abs((pos->next->x - pos->x) * (pos->next->next->y - pos->y) -
	       (pos->next->y - pos->y) * (pos->next->next->x - pos->x)) < 1e-8
	   && pos->prev != pos->next) {
      remove(pos->next);
      end = pos;
    }
    pos = pos->next;
  } while (pos != end);
}

static int count_line(outline *line) {
  outline *pos = line;
  int count = 0;
  do {
    pos = pos->next;
    ++count;
  } while (pos != line);
  return count;
}

static void process_path(BezierPath const *path, Mesh &out) {
  int i = 0;
  while (!(path[i].f[2] == 0.0 && path[i].f[3] == 0.0 &&
	   path[i].f[4] == 0.0 && path[i].f[5] == 0.0))
    ++i;
  int lines = i;
  int const subdiv_max = 16;
  outline tmp_line[lines * subdiv_max];
  int line_pos = 0;
  for (int i = 0 ; i < lines ; i++) {
    int subdiv = subdiv_max;
    if (path[i].f[0] == path[i].f[2] && path[i].f[1] == path[i].f[3] &&
	path[i].f[4] == path[i + 1].f[0] && path[i].f[5] == path[i + 1].f[1])
      subdiv = 1;
    for (int iu = 0 ; iu < subdiv ; ++iu) {
      double u = (double)iu / subdiv;
      double const t = 1.0 - u;
      assert(line_pos < lines * subdiv_max);
      double x = 
	path[i].f[0] * t * t * t
	+ 3 * u * t * (path[i].f[2] * t + path[i].f[4] * u)
	+ path[i + 1].f[0] * u * u * u;
      double y =
	path[i].f[1] * t * t * t
	+ 3 * u * t * (path[i].f[3] * t + path[i].f[5] * u)
	+ path[i + 1].f[1] * u * u * u;

      tmp_line[line_pos].x = x;
      tmp_line[line_pos].y = y;
      tmp_line[line_pos].prev = &tmp_line[line_pos - 1];
      tmp_line[line_pos].next = &tmp_line[line_pos + 1];
      tmp_line[line_pos].corner = (u == 0.0);
      line_pos++;
    }
  }

  tmp_line[0].prev = &tmp_line[line_pos - 1];
  tmp_line[line_pos - 1].next = &tmp_line[0];

  remove_useless(tmp_line);
  int start = 0;
  while (!tmp_line[start].next) ++start;

  if (tmp_line[start].next == tmp_line[start].prev) {
    printf("Empty shape!\n");
    return;
  }

  int count = count_line(&tmp_line[start]);
  Vector2dVector p;
  outline *pos = &tmp_line[start];
  for (int i = 0 ; i < count ; ++i) {
    p.push_back(Vector2d(pos->x, pos->y));
    pos = pos->next;
  }

  Vector2dVector result;
  Triangulate::Process(p, result);

  Point normal = Point(0.0, 0.0, 1.0);

  for (Vector2dVector::iterator t = result.begin() ;
       t != result.end() ; ++t) {
    out.addVertex(Point(t->GetX(), t->GetY(),
			shape_depth / 1000.0), normal);
    out.addColour(Colour(current_colour[0], current_colour[1], current_colour[2]));
  }
}

static Mesh create_mesh(BezierShape const *shapes) {
  Mesh out = Mesh::createEmpty();
  BezierShape const *shape = shapes;
  shape_depth = 0;
  while (shape->path) {
    current_colour = shape->colour;
    process_path(shape->path, out);
    ++shape_depth;
    ++shape;
  }
  return out;
}

static const char *vertex_shader =
"#version 150 compatibility\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_position;\n"
"in  vec4 in_colour;\n"
"out vec4 colour;\n"

"void main(void) {\n"
"    gl_Position = gl_ModelViewProjectionMatrix * vec4(in_position, 1.0);\n"
"    colour = in_colour;\n"
"}\n"
;

static const char *fragment_shader =
"#version 150\n"
"const float PI = 3.14159265;\n"
"uniform sampler2D texture;\n"
"uniform float alpha;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec4 colour;\n"
"out vec4 FragColor;\n"

"void main(void) {\n"
"    FragColor = colour * vec4(1.0, 1.0, 1.0, alpha);"
"}\n"
;


Image::Image(BezierShape *shapes)
  : GLObject(create_mesh(shapes), vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::COLOUR)),
    alpha(1.0)
{

}

void Image::render(double time) {
  render_init();
  glDisable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  uniform("alpha", alpha);
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  logErrors();
}
