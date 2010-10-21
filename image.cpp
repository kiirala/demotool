#include <assert.h>
//#include <string.h>
//#include <math.h>

//#include <signal.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "image.h"

struct outline {
  double x, y;
  bool corner;
  struct outline *next;
  struct outline *prev;
};

__attribute__((pure))
static bool cw(outline const * a,
	      outline const * b,
	      outline const * c)
{
  double area = a->x * b->y - a->y * b->x
    + a->y * c->x - a->x * c->y
    + b->x * c->y - c->x * b->y;
  return area <= 0.0;
}

__attribute__((pure))
static bool strict_cw(outline const * a,
		      outline const * b,
		      outline const * c)
{
  double area = a->x * b->y - a->y * b->x
    + a->y * c->x - a->x * c->y
    + b->x * c->y - c->x * b->y;
  return area < 0.0;
}

__attribute__((pure))
static bool empty_triangle(outline const * const corner) {
  outline const *t1 = corner->prev;
  outline const *t2 = corner;
  outline const *t3 = corner->next;

  outline const *point = t3->next;
  while (point != corner->prev) {
    if (strict_cw(t1, t2, point) && strict_cw(t2, t3, point) &&
	strict_cw(t3, t1, point))
      return false;
    point = point->next;
  }
  return true;
}

static int count_cw(outline const * line) {
  outline const *pos = line;
  int sum = 0;
  do {
    if (cw(pos->prev, pos, pos->next))
      ++sum;
    pos = pos->next;
  } while (pos != line);
  return sum;
}

static void remove(outline *pos) {
  pos->next->prev = pos->prev;
  pos->prev->next = pos->next;
  pos->prev = 0;
  pos->next = 0;
}

float const *current_colour;
int shape_depth;

static void triangulate(outline * line, Mesh &out) {
  Point normal = Point(0.0, 0.0, 1.0);
  //#ifdef DEBUG
  outline *last = line;
  //#endif
  
  while (line->next != line->prev) {
    assert(line->prev->next == line);
    assert(line->next->prev == line);
    if (count_cw(line) == 0) {
      printf("No cw-triangles left!\n");
      return;
    }
    if ((line->x - line->prev->x) * (line->next->x - line->prev->x) +
	       (line->y - line->prev->y) * (line->next->y - line->prev->y)
	       < 1e-4) {
      outline *next = line->next;
      remove(line);
      line = next;
    }
    else if (cw(line->prev, line, line->next) && empty_triangle(line)) {
      outline const *c[] = {line->next, line, line->prev};
      /*
      for (int i = 0 ; i < 3 ; i++) {
	int const a = i & 1;
	int const b = a + 1;
	if (c[a]->y > c[b]->y) {
	  c[a] = (outline const *)((intptr_t)c[a] ^ (intptr_t)c[b]);
	  c[b] = (outline const *)((intptr_t)c[a] ^ (intptr_t)c[b]);
	  c[a] = (outline const *)((intptr_t)c[a] ^ (intptr_t)c[b]);
	}
      }
      */
      for (int i = 0 ; i < 3 ; i++) {
	out.addVertex(Point(c[i]->x, c[i]->y, shape_depth / 1000.0), normal);
	out.addColour(Colour(current_colour[0], current_colour[1], current_colour[2]));
      }

      outline *next = line->next;
      remove(line);
      last = next;
      line = next;
    }
    else {
      line = line->next;
      /*
      if (line == last) {
	exit(10);
      }
      */
      //#endif
    }
  }
}

static bool winding(outline const *line) {
  outline const *l = line;
  outline const *high = line;
  do {
    if (l->y > high->y || (l->y == high->y && l->x > high->x))
      high = l;
    l = l->next;
  } while (l != line);

  bool ret = cw(high->prev, high, high->next);
  printf("winding: %s\n", ret ? "true" : "false");
  return ret;
}

static void remove_useless(outline *line) {
  outline *pos = line;
  outline *end = line;
  do {
    while (abs(pos->x - pos->next->x) < 1e0 &&
	   abs(pos->y - pos->next->y) < 1e0) {
      if (pos->next->corner) pos->corner = true;
      remove(pos->next);
      end = pos;
    }
    pos = pos->next;
  } while (pos != end);

  do {
    while (abs((pos->next->x - pos->x) * (pos->next->next->y - pos->y) -
	       (pos->next->y - pos->y) * (pos->next->next->x - pos->x)) < 1e-4
	   && pos->prev != pos->next) {
      remove(pos->next);
      end = pos;
    }
    pos = pos->next;
  } while (pos != end);
}

static void process_path(BezierPath const *path, Mesh &out) {
  int i = 0;
  while (!(path[i].f[2] == 0.0 && path[i].f[3] == 0.0 &&
	   path[i].f[4] == 0.0 && path[i].f[5] == 0.0))
    ++i;
  int lines = i;
  printf("lines: %d\n", lines);
  int const subdiv_max = 32;
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

  if (!winding(&tmp_line[start])) {
    //if (count_cw(&tmp_line[start]) < line_pos / 2) {
    for (int i = 0 ; i < line_pos ; ++i) {
      outline *tmp = tmp_line[i].prev;
      tmp_line[i].prev = tmp_line[i].next;
      tmp_line[i].next = tmp;
    }
  }

  triangulate(&tmp_line[start], out);
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

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec4 colour;\n"
"out vec4 FragColor;\n"

"void main(void) {\n"
"    FragColor = colour;"
"}\n"
;


Image::Image(BezierShape *shapes)
  : GLObject(create_mesh(shapes), vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::COLOUR))
{

}

void Image::render(double time) {
  render_init();
  glDisable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  logErrors();
}
