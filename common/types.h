#ifndef TT_TYPES_H
#define TT_TYPES_H

#include <cmath>

struct Colour {
  float r, g, b;

  Colour()
    : r(0.0f), g(0.0f), b(0.0f) { }
  Colour(float const r, float const g, float const b)
    : r(r), g(g), b(b) { }

  void set(float const r, float const g, float const b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }
};

struct Point {
  double x, y, z;

  Point(double const x = 0.0, double const y = 0.0, double const z = 0.0)
    : x(x), y(y), z(z) { }

  double length() const {
    return sqrt(x * x + y * y + z * z);
  }

  void normalize() {
    double length = sqrt(x * x + y * y + z * z);
    x /= length;
    y /= length;
    z /= length;
  }

  void set(double const x, double const y, double const z) {
    this->x = x;
    this->y = y;
    this->z = z;
  }

  Point& operator+=(Point const &other) {
    x = x + other.x;
    y = y + other.y;
    z = z + other.z;
    return *this;
  }

  Point& operator-=(Point const &other) {
    x = x - other.x;
    y = y - other.y;
    z = z - other.z;
    return *this;
  }

  const Point operator+ (Point const &other) const {
    Point res = *this;
    res += other;
    return res;
  }

  const Point operator- (Point const &other) const {
    Point res = *this;
    res -= other;
    return res;
  }

  const Point operator* (double const f) const {
    Point res(x * f, y * f, z * f);
    return res;
  }

  const Point operator/ (double const f) const {
    Point res(x / f, y / f, z / f);
    return res;
  }

  const Point cross(Point const &other) const {
    Point res;
    res.x = y * other.z - z * other.y;
    res.y = z * other.x - x * other.z;
    res.z = x * other.y - y * other.x;
    return res;
  }

  const Point at_length(double const new_length) const {
    double const factor = new_length / length();
    Point res = *this * factor;
    return res;
  }
};

struct Vertex {
  Point loc;
  Point normal;

  Vertex()
    : loc(), normal() { }
  Vertex(Point const &loc, Point const &normal)
    : loc(loc), normal(normal) { }
};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TT_TYPES_H */
