#include <assert.h>
//#include <string.h>
//#include <math.h>

//#include <signal.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "image.h"

struct outline {
  double x, y;
  struct outline *next;
  struct outline *prev;
};

__attribute__((pure))
static int cw(outline const * a,
	      outline const * b,
	      outline const * c)
{
  int area = a->x * b->y - a->y * b->x
    + a->y * c->x - a->x * c->y
    + b->x * c->y - c->x * b->y;
  return area <= 0;
}

__attribute__((pure))
static int empty_triangle(outline const * const corner) {
  outline const *t1 = corner->prev;
  outline const *t2 = corner;
  outline const *t3 = corner->next;

  outline const *point = t3->next;
  while (point != corner->prev) {
    if (cw(t1, t2, point) && cw(t2, t3, point) && cw(t3, t1, point))
      return 0;
    point = point->next;
  }
  return 1;
}

float *current_colour;

static void triangulate(outline * line, int const segments,
			Mesh &out) {
  Point normal = Point(0.0, 0.0, 1.0);
#ifdef DEBUG
  outline *last = line;
#endif

  while (line->next != line->prev) {
    assert(line->prev->next == line);
    assert(line->next->prev == line);
    if (cw(line->prev, line, line->next) && empty_triangle(line)) {
      outline const *c[] = {line->next, line, line->prev};

      for (int i = 0 ; i < 3 ; i++) {
	int const a = i & 1;
	int const b = a + 1;
	if (c[a]->y > c[b]->y) {
	  c[a] = (outline const *)((intptr_t)c[a] ^ (intptr_t)c[b]);
	  c[b] = (outline const *)((intptr_t)c[a] ^ (intptr_t)c[b]);
	  c[a] = (outline const *)((intptr_t)c[a] ^ (intptr_t)c[b]);
	}
      }

      for (int i = 0 ; i < 3 ; i++) {
	out.addVertex(Point(c[i]->x, c[i]->y), normal);
	out.addColour(Colour(current_colour[0], current_colour[1], current_colour[2]));
      }

      line->next->prev = line->prev;
      line->prev->next = line->next;
#ifdef DEBUG
      last = line->next;
      line = line->next;
    } else {
      line = line->next;
      if (line == last) {
	exit(10);
      }
#endif
    }
#ifndef DEBUG
    line = line->next;
#endif
  }

  output->triangle_count = out_num;
}

static void process_path(BezierPath const *path, Mesh &out) {
  int i = 0;
  while (!(path[i].f[2] == 0.0 && path[i].f[3] == 0.0 &&
	   path[i].f[4] == 0.0 && path[i].f[5] == 0.0))
    ++i;
  int lines = i;
  const int subdiv = 32;
  outline tmp_line[lines * subdiv];
  int line_pos = 0;
  for (int i = 0 ; i < lines ; i++) {
    for (double u = 0.0 ; u < 1.0 ; u += 1.0 / subdiv) {
      double const t = 1.0 - u;
      assert(line_pos < bezier_max);
      tmp_line[line_pos].x = 
	in[i].f[0] * t * t * t
	/*
	  + in[i].f[2] * 3 * u * t * t
	  + in[i].f[4] * 3 * u * u * t
	*/
	+ 3 * u * t * (in[i].f[2] * t + in[i].f[4] * u)
	+ in[i + 1].f[0] * u * u * u;
      tmp_line[line_pos].y =
	in[i].f[1] * t * t * t
	/*
	  + in[i].f[3] * 3 * u * t * t
	  + in[i].f[5] * 3 * u * u * t
	*/
	+ 3 * u * t * (in[i].f[3] * t + in[i].f[5] * u)
	+ in[i + 1].f[1] * u * u * u;

      tmp_line[line_pos].prev = &tmp_line[line_pos - 1];
      tmp_line[line_pos].next = &tmp_line[line_pos + 1];
      line_pos++;
    }
  }

  tmp_line[0].prev = &tmp_line[line_pos - 1];
  tmp_line[line_pos - 1].next = &tmp_line[0];

  return triangulate(tmp_line, line_pos);
}

static Mesh create_mesh(BezierShape const *shapes) {
  Mesh out = Mesh::createEmpty();
  BezierShape *shape = shapes;
  while (shape->path) {
    current_colour = shape->colour;
    Mesh m = process_path(shape->path, out);
    ++shape;
  }
  return out;
}

Image::Image(BezierShape *shapes)
  : GLObject(create_mesh(shapes), vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::position | GLObject::colour)),
    shapes()
{
  BezierPath *shape = shapes;
  while (shape->path) {
    Mesh m = createMesh(shape->path);
    m.setColour(shape->colour[0], shape->colour[1], shape->colour[2]);
  }
}

Image::render(double time) {
  render_init();
  glDisable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  for (std::vector<Mesh>::iterator i = shapes.begin() ;
       i != shapes.end() ; ++i) {
    uniform3("colour", i->colourArray());
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
    logErrors();
  }
}
