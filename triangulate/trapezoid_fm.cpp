/*
 * Decomposing polygon into trapezoids
 * Implents algorithm published by A. Fournier, D. Y. Montuno in
 * paper "Triangulating Simple Polygons and Equivalent Problems"
 * ACM Transactions on Graphics, Vol. 3, No. 2, April 1984
 *
 * Niko Kiirala <niko@kiirala.com> 2008
 */

#include <assert.h>
#include <set>
#include <vector>
#include <algorithm>
#include <map>
#include <list>
#include <utility>

#include "trapezoid_fm.h"
#include "types.h"
#include "output_helper.h"

std::vector<TVertex*> convert_to_tvertex(std::vector<Vertex> const &v) {
  std::vector<TVertex*> out;
  std::map<Vertex const *,TVertex*> links;

  for (std::vector<Vertex>::const_iterator i = v.begin() ;
       i != v.end() ; ++i) {
    TVertex *add = new TVertex(*i);
    out.push_back(add);
    links.insert(std::make_pair(i->prev->next, out.back()));
  }

  assert(v.size() == out.size());

  /*
  for (std::map<Vertex const *, TVertex*>::const_iterator i = links.begin() ;
       i != links.end() ; i++) {
  }
  */

  for (size_t i = 0 ; i < out.size() ; ++i) {
    out[i]->next = links.find(v[i].next)->second;
    out[i]->prev = links.find(v[i].prev)->second;
    out[i]->orig_next = out[i]->next;
    out[i]->orig_prev = out[i]->prev;
    assert(out[i]->next);
    assert(out[i]->prev);
  }

  for (size_t i = 0 ; i < out.size() ; ++i) {
    assert(v[i].x == out[i]->x);
    assert(v[i].y == out[i]->y);
    assert(out[i]->self == out[i]->prev->next);
    assert(out[i]->self == out[i]->next->prev);
    assert(v[i].prev->x == out[i]->prev->x);
    assert(v[i].prev->y == out[i]->prev->y);
    assert(v[i].next->x == out[i]->next->x);
    assert(v[i].next->y == out[i]->next->y);
  }

  return out;
}

bool tvertex_cmp(TVertex const *a, TVertex const *b) {
  return *a < *b;
}

void insert_trapezoid(Trapezoid *t) {
  TVertex *top = t->top;
  assert((!top->tr1 && !top->tr2) || (top->tr1 && !top->tr2));

  if (top->tr1) {
    if (*top->tr1 < *t) {
      top->tr2 = t;
    }
    else {
      top->tr2 = top->tr1;
      top->tr1 = t;
    }
  }
  else {
    top->tr1 = t;
  }
}

bool is_type_1(TVertex const *v) {
  int ydiff1 = v->prev->y - v->y;
  int ydiff2 = v->next->y - v->y;

  if ((ydiff1 < 0 && ydiff2 > 0) || (ydiff1 > 0 && ydiff2 < 0)) return true;
  if (ydiff1 == 0 && ydiff2 == 0) return true;
  if (ydiff1 == 0) {
    int xdiff1 = v->prev->x - v->x;
    if ((ydiff2 > 0 && xdiff1 > 0) || (ydiff2 < 0 && xdiff1 < 0))
      return true;
  }
  if (ydiff2 == 0) {
    int xdiff2 = v->next->x - v->x;
    if ((ydiff1 > 0 && xdiff2 > 0) || (ydiff1 < 0 && xdiff2 < 0))
      return true;
  }
  return false;
}

std::vector<TVertex*> trapezoid_decompose(Polygon const &p) {
  std::set<Trapezoid> active;

  std::vector<TVertex*> polygon = convert_to_tvertex(p);
  std::sort(polygon.begin(), polygon.end(), tvertex_cmp);

  for (std::vector<TVertex*>::iterator i = polygon.begin();
       i != polygon.end() ; i++) {
    TVertex * const i_ptr = *i;
    assert((*i)->next->prev == (*i)->prev->next);

    Trapezoid const dummy(i_ptr);
    std::set<Trapezoid>::iterator const tr = active.lower_bound(dummy);

    if (is_type_1(i_ptr)) {
      /* Type 1: border vertex */
      if (tr == active.end()) {
	std::set<Trapezoid>::iterator t = tr;
	printf("l:(% 2d, % 2d)(% 2d, % 2d) r:(% 2d, %2d)(% 2d, % 2d) top:(% 2d, % 2d)\n",
	       t->left->a->x, t->left->a->y,
	       t->left->b->x, t->left->b->y,
	       t->right->a->x, t->right->a->y,
	       t->right->b->x, t->right->b->y,
	       t->top->x, t->top->y);
	assert(false);
      }
      assert(tr != active.end());
      assert(tr->left->b == i_ptr || tr->right->b == i_ptr);

      Trapezoid *to_finish = new Trapezoid(*tr);
      to_finish->bot = i_ptr;
      insert_trapezoid(to_finish);

      Trapezoid to_insert(i_ptr);
      if (tr->left->b == i_ptr) {
	to_insert.left = new Edge(i_ptr, (*i)->prev);
	to_insert.right = tr->right;
      }
      else {
	assert(tr->right->b == i_ptr);
	to_insert.left = tr->left;
	to_insert.right = new Edge(i_ptr, (*i)->next);
      }
      active.erase(tr);
      active.insert(to_insert);
      //trapezoid_dbg(1, to_insert);
    }
    else if ((*i)->prev->y - (*i)->y < 0 || (*i)->next->y - (*i)->y < 0) {
      /* Type 2: both edges downwards */
      assert(tr == active.end() || !(*tr < dummy));
      if (tr != active.end() && !(*tr > dummy)) {
	/* Inside an active trapezoid: divide into two */
	Trapezoid *to_finish = new Trapezoid(*tr);
	to_finish->bot = i_ptr;
	insert_trapezoid(to_finish);

	Trapezoid new1(i_ptr), new2(i_ptr);
	new1.left = tr->left;
	new1.right = new Edge(i_ptr, (*i)->next);
	new2.left = new Edge(i_ptr, (*i)->prev);
	new2.right = tr->right;

	active.erase(tr);
	active.insert(new1);
	active.insert(new2);
	//trapezoid_dbg(2, new1);
	//trapezoid_dbg(2, new2);
      }
      else {
	/* Outside an active trapezoid: start a new trapezoid */
	Trapezoid to_insert(i_ptr);
	assert(((*i)->prev->x - i_ptr->x) * ((*i)->next->y - i_ptr->y)
	       - ((*i)->next->x - i_ptr->x) * ((*i)->prev->y - i_ptr->y)
	       > 0); // Kiertää takaperin
	to_insert.left = new Edge(i_ptr, (*i)->prev);
	to_insert.right = new Edge(i_ptr, (*i)->next);
	active.insert(to_insert);
	//trapezoid_dbg(2, to_insert);
      }
    }
    else if ((*i)->prev->y - (*i)->y > 0 || (*i)->next->y - (*i)->y > 0) {
      /* Type 3: both edges upwards */
      assert(tr != active.end());
      if (tr->left->b == tr->right->b) {
	/* End of an trapezoid */
	Trapezoid *to_finish = new Trapezoid(*tr);
	to_finish->bot = i_ptr;
	insert_trapezoid(to_finish);
	active.erase(tr);
      }
      else {
	/* Combine two trapezoids into one */
	std::set<Trapezoid>::iterator tr2 = tr;
	tr2++;
	assert(tr2 != active.end());

	Trapezoid *finish1 = new Trapezoid(*tr);
	finish1->bot = i_ptr;
	insert_trapezoid(finish1);

	Trapezoid *finish2 = new Trapezoid(*tr2);
	finish2->bot = i_ptr;
	insert_trapezoid(finish2);

	assert(tr->right->b == i_ptr && tr2->left->b == i_ptr);

	Trapezoid to_insert(i_ptr);
	to_insert.left = tr->left;
	to_insert.right = tr2->right;

	tr2++;
	active.erase(tr, tr2);
	active.insert(to_insert);
	//trapezoid_dbg(3, to_insert);
      }
    }
    else {
      assert(false);
    }
  }

  return polygon;
}

void remove_trapezoid(TVertex *v) {
  if (v->tr2) {
    assert(v->tr1);
    delete v->tr1;
    v->tr1 = v->tr2;
    v->tr2 = 0;
  }
  else if (v->tr1) {
    assert(!v->tr2);
    delete v->tr1;
    v->tr1 = 0;
  }
}

TVertex *trapezoid_diagonal(TVertex *v) {
  if (v->tr1 && !(v->tr1->bot == v->orig_next || v->tr1->bot == v->orig_prev)) {
    return v->tr1->bot;
  }
  return 0;
}

bool is_convex(Point const &a, Point const &b, Point const &c) {
  int bax = a.x - b.x;
  int bay = a.y - b.y;
  int bcx = c.x - b.x;
  int bcy = c.y - b.y;
  int cross = bax * bcy - bay * bcx;

  return cross >= 0;
}

std::list<Point> make_vertex_list(TVertex const *begin, TVertex const *end) {
  std::list<Point> out;
  TVertex const *cur = begin;

  do {
    Point add(cur->x, cur->y);
    out.push_back(add);
    cur = cur->next;
  } while (cur != end->next);
  
  return out;
}

void triangulate_monotone(TVertex const *first, TVertex const *last,
			  Triangulation &out) {
  std::list<Point> poly = make_vertex_list(first, last);
  std::list<Point>::iterator top = poly.begin();
  std::list<Point>::iterator bottom = poly.begin();
  std::list<Point>::iterator cur = poly.begin();

  int vertex_count = 0;
  do {
    if (cur->y > top->y || (cur->y == top->y && cur->x < top->x)) {
      top = cur;
    }
    if (cur->y < bottom->y || (cur->y == bottom->y && cur->x > bottom->x)) {
      bottom = cur;
    }
    ++cur;
    vertex_count++;
  } while (cur != poly.end());
  std::list<Point>::iterator tmp = top;
  if (tmp == poly.begin()) tmp = poly.end();
  --tmp;
  std::list<Point>::iterator start = (tmp == bottom) ? top : bottom;

  cur = start;
  ++cur;
  if (cur == poly.end()) cur = poly.begin();
  while(vertex_count >= 3) {
    std::list<Point>::iterator prev = cur;
    if (prev == poly.begin()) prev = poly.end();
    --prev;
    std::list<Point>::iterator next = cur; 
    ++next;
    if (next == poly.end()) next = poly.begin();

    assert(prev != next && cur != prev && cur != next);

    if (cur != top && cur != bottom && is_convex(*prev, *cur, *next)) {
      Triangle tri(*prev, *cur, *next);
      out.insert(tri);
      printf("inserted: (%d, %d) (%d, %d) (%d, %d)\n",
	     prev->x, prev->y,
	     cur->x, cur->y,
	     next->x, next->y);
      std::list<Point>::iterator save = cur;
      if (save == poly.begin()) save = poly.end();
      --save;
      assert(top != cur);
      assert(bottom != cur);
      assert(save != cur);
      assert(cur != poly.end());
      poly.erase(cur);
      /*
      if (cur == first) cur = first->next;
      else cur = save;
      */
      cur = save;
      vertex_count--;
      assert((size_t)vertex_count == poly.size());
    }
    else {
      ++cur;
      if (cur == poly.end()) cur = poly.begin();
    }
  }
  /*
  top->prev->next = top->next;
  top->next->prev = top->prev;
  bottom->prev->next = bottom->next;
  bottom->next->prev = bottom->prev;
  */
}

void triangulate(TVertex *first, TVertex *last, Triangulation &out) {
  TVertex *current = first;
  printf("Triangulate (%d, %d) -- (%d, %d)\n",
	 first->x, first->y, last->x, last->y);
  while (!current->done) {
    current->done = true;
    TVertex *bottom = trapezoid_diagonal(current);
    if (bottom) {
      TVertex *save_next = current->next;
      TVertex *save_prev = bottom->prev;
      printf("Leikkaus (%d, %d) (%d, %d)\n", current->x, current->y,
	     bottom->x, bottom->y);
      current->next = bottom;
      bottom->prev = current;
      remove_trapezoid(current);
      triangulate(bottom, current, out);

      current->done = false;
      bottom->done = false;
      current->next = save_next;
      bottom->prev = save_prev;
      bottom->next = current;
      current->prev = bottom;
      triangulate(current, bottom, out);
      return;
    }
    else {
      current = current->next;
    }
  }
  triangulate_monotone(first, last, out);
}

Triangulation triangulate_trapezoids(std::vector<TVertex*> &t) {
  Triangulation out;
  triangulate(t.front(), t.back(), out);
  return out;
}

