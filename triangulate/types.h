#ifndef TRIANGULATE_TYPES_H
#define TRIANGULATE_TYPES_H

#include <vector>
#include <map>
#include <set>
#include <assert.h>

struct Point {
  int x, y;

  Point() : x(0), y(0) { }
  Point(int x, int y) : x(x), y(y) { }
};

struct Vertex {
  int x, y;
  Vertex *next, *prev;

  Vertex() : x(0), y(0), next(0), prev(0) { }
  Vertex(int x, int y) : x(x), y(y), next(0), prev(0) { }

  bool operator< (Vertex const &other) const {
    if (y == other.y) {
      return x < other.x;
    }
    return y > other.y;
  }

  bool operator==(Vertex const &other) const {
    return x == other.x && y == other.y;
  }
};

struct Trapezoid;
struct TVertex {
  int x, y;
  TVertex *self, *next, *prev, *orig_next, *orig_prev;
  Trapezoid *tr1, *tr2;
  bool done;

  TVertex()
    : x(0), y(0), self(this), next(0), prev(0), tr1(0), tr2(0), done(false)
  { }
  explicit TVertex(Vertex const &v)
    : x(v.x), y(v.y), self(this), next(0), prev(0), tr1(0), tr2(0), done(false)
  { }

  bool operator< (TVertex const &other) const {
    if (y == other.y) {
      return x < other.x;
    }
    return y > other.y;
  }
};

struct Edge {
  TVertex *a, *b;

  Edge() : a(0), b(0) { }
  Edge(TVertex *a, TVertex *b) : a(a), b(b) { }
};

typedef std::vector<Vertex> Polygon;

struct Triangle {
  int v[6];

  Triangle() { }
  Triangle(TVertex *a, TVertex *b, TVertex *c)
  {
    v[0] = a->x;
    v[1] = a->y;
    v[2] = b->x;
    v[3] = b->y;
    v[4] = c->x;
    v[5] = c->y;
  }

  Triangle(Vertex const &a, Vertex const &b, Vertex const &c)
  {
    v[0] = a.x;
    v[1] = a.y;
    v[2] = b.x;
    v[3] = b.y;
    v[4] = c.x;
    v[5] = c.y;
  }

  Triangle(Point const &a, Point const &b, Point const &c)
  {
    v[0] = a.x;
    v[1] = a.y;
    v[2] = b.x;
    v[3] = b.y;
    v[4] = c.x;
    v[5] = c.y;
  }
};

struct Triangulation {
  std::vector<Triangle> triangles;
  std::map<std::pair<int,int>, std::set<Triangle*> > triangle_map;

  Triangulation() { }

  Triangulation(Triangulation const &other)
    : triangles(other.triangles), triangle_map()
  {
    for (std::vector<Triangle>::iterator i = triangles.begin() ;
	 i != triangles.end() ; ++i) {
      triangle_map[std::make_pair(i->v[0], i->v[1])].insert(&(*i));
      triangle_map[std::make_pair(i->v[2], i->v[3])].insert(&(*i));
      triangle_map[std::make_pair(i->v[4], i->v[5])].insert(&(*i));
    }
  }

  void insert(Triangle const &t) {
    triangles.push_back(t);
    triangle_map[std::make_pair(t.v[0], t.v[1])].insert(&triangles.back());
    triangle_map[std::make_pair(t.v[2], t.v[3])].insert(&triangles.back());
    triangle_map[std::make_pair(t.v[4], t.v[5])].insert(&triangles.back());
  }
};

struct Trapezoid {
  TVertex *top, *bot;
  Edge *left, *right;

  Trapezoid()
    : top(0), bot(0), left(0), right(0)
  { }
  Trapezoid(TVertex *v)
    : top(v), bot(0), left(0), right(0)
  { }

  bool operator>(Trapezoid const &other) const {
    if (top->y == other.top->y) {
      if (top->x == other.top->x) {
	return left->a == top;
      }
      return top->x > other.top->x &&
	left->b->x > other.top->x;
    }
    
    if (top->y < other.top->y) return other < *this;
    
    assert(left->b->y <= left->a->y);
    int x1 = left->b->x - left->a->x;
    int y1 = left->b->y - left->a->y;
    int x2 = other.top->x - left->a->x;
    int y2 = other.top->y - left->a->y;
    
    int cross = x1 * y2 - y1 * x2;
    
    return cross < 0;
  }
  
  bool operator<(Trapezoid const &other) const {
    if (top->y == other.top->y) {
      if (top->x == other.top->x) {
	return right->a == top;
      }
      return top->x < other.top->x &&
	right->b->x < other.top->x;
    }
    
    if (top->y < other.top->y) return other > *this;
    
    assert(right->b->y <= right->a->y);
    int x1 = right->b->x - right->a->x;
    int y1 = right->b->y - right->a->y;
    int x2 = other.top->x - right->a->x;
    int y2 = other.top->y - right->a->y;
    
    int cross = x1 * y2 - y1 * x2;
    
    return cross > 0;
  }
};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TRIANGULATE_TYPES_H */
