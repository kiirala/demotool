#include <vector>
#include <cmath>
#include <cstdio>
#include <cassert>
#include <GL/gl.h>

#include "mesh.h"
#include "matrix.h"
#include "types.h"
#include "logger.h"

Mesh::Mesh()
  : type(TRIANGLES), vertices(), v_array(0), n_array(0), texcoord_array(0)
{
  colour.set(0.0, 0.0, 0.0);
}

Mesh::~Mesh() {
  delete [] v_array;
  delete [] n_array;
  delete [] texcoord_array;
}

void Mesh::render() const {
  glColor3f(colour.r, colour.g, colour.b);

  if (type == TRIANGLES) {
    glBegin(GL_TRIANGLES);
  }
  else if (type == TRIANGLE_FAN) {
    glBegin(GL_TRIANGLE_FAN);
  }
  else if (type == TRIANGLE_STRIP) {
    glBegin(GL_TRIANGLE_STRIP);
  }
  else {
    error("Unknown mesh type");
    return;
  }

  for (std::vector<Vertex>::const_iterator i = vertices.begin() ;
       i != vertices.end() ; i++) {
    const Vertex &v = *i;
    glNormal3d(v.normal.x, v.normal.y, v.normal.z);
    glVertex3d(v.loc.x, v.loc.y, v.loc.z);
  }

  glEnd();
}

void Mesh::transform(Matrix const &trans) {
  Matrix normal_trans = trans.inverse().transpose();

  for (std::vector<Vertex>::iterator i = vertices.begin() ;
       i != vertices.end() ; i++) {
    (*i).loc = trans * (*i).loc;
    (*i).normal = normal_trans * (*i).normal;
  }
}

int Mesh::vertexCount() {
  return vertices.size();
}

GLfloat* Mesh::vertexArray() {
  if (!v_array) v_array = new GLfloat[vertexCount() * 3];

  int a_pos = 0;
  for (std::vector<Vertex>::iterator i = vertices.begin() ;
       i != vertices.end() ; i++) {
    v_array[a_pos] = (*i).loc.x;
    v_array[a_pos + 1] = (*i).loc.y;
    v_array[a_pos + 2] = (*i).loc.z;
    a_pos += 3;
  }
  return v_array;
}

GLfloat* Mesh::normalArray() {
  if (!n_array) n_array = new GLfloat[vertexCount() * 3];

  int a_pos = 0;
  for (std::vector<Vertex>::iterator i = vertices.begin() ;
       i != vertices.end() ; i++) {
    n_array[a_pos] = (*i).normal.x;
    n_array[a_pos + 1] = (*i).normal.y;
    n_array[a_pos + 2] = (*i).normal.z;
    a_pos += 3;
  }
  return n_array;
}

GLfloat* Mesh::texcoordArray() {
  if (!texcoord_array) {
    if (texcoords.size() == 0)
      return 0;
    assert(texcoords.size() == vertices.size());
    texcoord_array = new GLfloat[vertexCount() * 2];
  }

  int a_pos = 0;
  for (std::vector<Point>::iterator i = texcoords.begin() ;
       i != texcoords.end() ; i++) {
    texcoord_array[a_pos] = (*i).x;
    texcoord_array[a_pos + 1] = (*i).y;
    a_pos += 2;
  }
  return texcoord_array;
}

Mesh Mesh::createSquare() {
  Mesh m;
  m.type = TRIANGLES;
  m.vertices.reserve(6);
  m.texcoords.reserve(6);

  Point normal(0, 0, 1);
  m.vertices.push_back(Vertex(Point(0, 0), normal));
  m.texcoords.push_back(Point(0, 0));
  m.vertices.push_back(Vertex(Point(0, 1), normal));
  m.texcoords.push_back(Point(0, 1));
  m.vertices.push_back(Vertex(Point(1, 0), normal));
  m.texcoords.push_back(Point(1, 0));

  m.vertices.push_back(Vertex(Point(1, 0), normal));
  m.texcoords.push_back(Point(1, 0));
  m.vertices.push_back(Vertex(Point(0, 1), normal));
  m.texcoords.push_back(Point(0, 1));
  m.vertices.push_back(Vertex(Point(1, 1), normal));
  m.texcoords.push_back(Point(1, 1));

  return m;
}

/**
 * Luo sylinterin ilman päätyjä. Toinen pää on origossa, toinen pisteessä
 * (0, length, 0)
 */
Mesh Mesh::createCylinder(double const length, double const radius,
			  int const slices)
{
  Mesh m;
  m.type = TRIANGLE_STRIP;
  m.vertices.reserve(slices * 2 + 2);

  for (int i = 0 ; i <= slices ; i++) {
    double angle = (2 * M_PI / slices) * i;
    double x = cos(angle) * radius;
    double z = sin(angle) * radius;

    Point normal(x, 0.0, z);
    normal.normalize();

    Vertex a(Point(x, 0.0, z), normal);
    m.vertices.push_back(a);

    Vertex b(Point(x, length, z), normal);
    m.vertices.push_back(b);
  }

  return m;
}

/**
 * Luo ympyrän, jonka keskipiste on origossa. Vastapäivään kiertävän
 * ympyrän normaali on positiivisen y-akselin suuntaan, myötäpäivään
 * kiertävän negatiivisen y-akselin suuntaan.
 */
Mesh Mesh::createDisk(double const radius, int const slices,
		      Direction const dir)
{
  Mesh m;
  m.type = TRIANGLE_FAN;
  m.vertices.reserve(slices + 2);

  Point normal(0.0, 1.0, 0.0);
  if (dir == CW) normal.y = -1.0;

  Vertex center(Point(0.0, 0.0, 0.0), normal);
  m.vertices.push_back(center);

  double mult = 1.0;
  if (dir == CCW) mult = -1.0;

  for (int i = 0 ; i <= slices ; i++) {
    double angle = (2 * M_PI / slices) * i;
    double x = cos(angle) * radius;
    double z = sin(angle) * radius * mult;

    Vertex a(Point(x, 0.0, z), normal);
    m.vertices.push_back(a);
  }

  return m;
}

/**
 * Luo kartion ilman päätyjä. Toinen pää on origossa, toinen pisteessä
 * (0, length, 0)
 */
Mesh Mesh::createCone(double const length, double const radius1,
		      double const radius2, int const slices) {
  Mesh m;
  m.type = TRIANGLE_STRIP;
  m.vertices.reserve(slices * 2 + 2);

  for (int i = 0 ; i <= slices ; i++) {
    double angle = (2 * M_PI / slices) * i;
    double x1 = cos(angle) * radius1;
    double z1 = sin(angle) * radius1;
    double x2 = cos(angle) * radius2;
    double z2 = sin(angle) * radius2;

    Point normal(x1, (x1*x1 - x1*x2 + z1*z1 - z1*z2)/ length, z1);
    normal.normalize();

    Vertex a(Point(x1, 0.0, z1), normal);
    m.vertices.push_back(a);

    Vertex b(Point(x2, length, z2), normal);
    m.vertices.push_back(b);
  }

  return m;
}

static bool singularity(Point const &a) {
  if (a.y == -M_PI / 2.0 || a.y == M_PI / 2.0)
    return true;
  return false;
}

static Point tc_mid(Point const &a, Point const &b) {
  Point ca = Point(cos(a.x) * cos(a.y),
		   sin(a.x) * cos(a.y),
		   sin(a.y));
  Point cb = Point(cos(b.x) * cos(b.y),
		   sin(b.x) * cos(b.y),
		   sin(b.y));
  Point mid = (ca + cb) / 2.0;
  Point spherical(atan2(mid.y, mid.x), asin(mid.z / mid.length()));
  if (spherical.x < a.x && spherical.x < b.x && (a.x >= M_PI || b.x >= M_PI))
    spherical.x += 2.0 * M_PI;
  return spherical;
  /*
  double mid_x = (a.x + b.x) / 2.0;
  double mid_y = (a.y + b.y) / 2.0;
  if (singularity(a))
    mid_x = b.x;
  else if (singularity(b))
    mid_x = a.x;
  return Point(mid_x, mid_y);
  */
}

Mesh Mesh::createBall(double const radius, int const subdivisions) {
  Mesh m;
  m.type = TRIANGLES;

  if (subdivisions < 0) {
    error("Subdivisions < 0");
  }

  if (subdivisions == 0) {
    double length = radius / sqrt(2);
    Point tc_a(0.0, M_PI / 2.0);
    Point tc_b(-M_PI + M_PI / 4, 0.0);
    Point tc_c(-M_PI / 4, 0.0);
    Point step(M_PI / 2, 0.0);

    for (int i = 0 ; i < 4 ; i++) {
      Point a(0.0, 0.0, radius);
      Point b((i == 0 || i == 3) ? -length : length,
	      (i <= 1) ? -length : length,
	      0.0);
      Point c((i <= 1) ? length : -length,
	      (i == 0 || i == 3) ? -length : length,
	      0.0);
      Point normal = (b - a).cross(c - a);

      Vertex va = Vertex(a, normal);
      m.vertices.push_back(va);
      m.texcoords.push_back(tc_a);
      Vertex vb = Vertex(b, normal);
      m.vertices.push_back(vb);
      m.texcoords.push_back(tc_b);
      Vertex vc = Vertex(c, normal);
      m.vertices.push_back(vc);
      m.texcoords.push_back(tc_c);
      tc_b += step;
      tc_c += step;
    }

    tc_a.y = -M_PI / 2.0;
    tc_b.x = -M_PI / 4.0;
    tc_c.x = -M_PI + M_PI / 4;
    for (int i = 0 ; i < 4 ; i++) {
      Point a(0.0, 0.0, -radius);
      Point b((i <= 1) ? length : -length,
	      (i == 0 || i == 3) ? -length : length,
	      0.0);
      Point c((i == 0 || i == 3) ? -length : length,
	      (i <= 1) ? -length : length,
	      0.0);
      // TODO: palloahan tässä oltiin tekemässä, joka verteksille pitäisi
      // olla oma normaali
      Point normal = (b - a).cross(c - a);

      Vertex va = Vertex(a, normal);
      m.vertices.push_back(va);
      m.texcoords.push_back(tc_a);
      Vertex vb = Vertex(b, normal);
      m.vertices.push_back(vb);
      m.texcoords.push_back(tc_b);
      Vertex vc = Vertex(c, normal);
      m.vertices.push_back(vc);
      m.texcoords.push_back(tc_c);
      tc_b += step;
      tc_c += step;
    }
  }
  else {
    Mesh prev = createBall(radius, subdivisions - 1);
    std::vector<Vertex>::const_iterator i = prev.vertices.begin();
    std::vector<Point>::const_iterator tc = prev.texcoords.begin();
    while (i != prev.vertices.end()) {
      Vertex a = *i; ++i;
      Vertex b = *i; ++i;
      Vertex c = *i; ++i;
      Point tc_a = *tc; ++tc;
      Point tc_b = *tc; ++tc;
      Point tc_c = *tc; ++tc;

      Point mid_ab = ((a.loc + b.loc) / 2.0).at_length(radius);
      Point mid_ac = ((a.loc + c.loc) / 2.0).at_length(radius);
      Point mid_bc = ((b.loc + c.loc) / 2.0).at_length(radius);

      Point tc_ab = tc_mid(tc_a, tc_b);
      Point tc_ac = tc_mid(tc_a, tc_c);
      Point tc_bc = tc_mid(tc_b, tc_c);
      if (singularity(tc_a)) tc_a.x = tc_bc.x;
      if (singularity(tc_b)) tc_b.x = tc_ac.x;
      if (singularity(tc_c)) tc_c.x = tc_ab.x;
      /*
      debug("ac diff: %.4f %4f %4f",
	    cos(tc_ac.x) * cos(tc_ac.y) - mid_ac.x,
	    sin(tc_ac.x) * cos(tc_ac.y) - mid_ac.y,
	    sin(tc_ac.y) - mid_ac.z);
      */

      m.vertices.push_back(Vertex(a.loc, a.loc.at_length(1)));
      m.vertices.push_back(Vertex(mid_ab, mid_ab.at_length(1)));
      m.vertices.push_back(Vertex(mid_ac, mid_ac.at_length(1)));
      m.texcoords.push_back(tc_a);
      m.texcoords.push_back(tc_ab);
      m.texcoords.push_back(tc_ac);

      m.vertices.push_back(Vertex(mid_ab, mid_ab.at_length(1)));
      m.vertices.push_back(Vertex(b.loc, b.loc.at_length(1)));
      m.vertices.push_back(Vertex(mid_bc, mid_bc.at_length(1)));
      m.texcoords.push_back(tc_ab);
      m.texcoords.push_back(tc_b);
      m.texcoords.push_back(tc_bc);

      m.vertices.push_back(Vertex(mid_ab, mid_ab.at_length(1)));
      m.vertices.push_back(Vertex(mid_bc, mid_bc.at_length(1)));
      m.vertices.push_back(Vertex(mid_ac, mid_ac.at_length(1)));
      m.texcoords.push_back(tc_ab);
      m.texcoords.push_back(tc_bc);
      m.texcoords.push_back(tc_ac);

      m.vertices.push_back(Vertex(mid_ac, mid_ac.at_length(1)));
      m.vertices.push_back(Vertex(mid_bc, mid_bc.at_length(1)));
      m.vertices.push_back(Vertex(c.loc, c.loc.at_length(1)));
      m.texcoords.push_back(tc_ac);
      m.texcoords.push_back(tc_bc);
      m.texcoords.push_back(tc_c);
    }
  }

  return m;
}
