//#include <vector>
//#include <string>
//#include <iostream>
//#include <fstream>
#include <cmath>
#include <cstdio>
#include <cassert>
#include <stdlib.h>
#include <GL/gl.h>

#include "mesh.h"
#include "matrix.h"
#include "types.h"
#include "logger.h"

VertexList::VertexList()
  : items((Vertex*)malloc(sizeof(Vertex) * 8)), count(0), capacity(8)
{ }

VertexList::VertexList(VertexList const &other)
  : items((Vertex*)malloc(sizeof(Vertex) * 8)), count(0), capacity(8)
{
  for (int i = 0 ; i < other.count ; ++i) {
    this->addVertex(other.items[i]);
  }
}

VertexList::~VertexList() {
  free(items);
}

void VertexList::addVertex(Vertex const &v) {
  if (count == capacity) {
    Vertex *new_items = (Vertex*)malloc(sizeof(Vertex) * capacity * 2);
    for (int i = 0 ; i < count ; ++i) {
      new_items[i] = items[i];
    }
    free(items);
    capacity *= 2;
    items = new_items;
  }
  items[count] = v;
  count++;
}

Mesh::Mesh()
  : type(TRIANGLES), vertices(),
    v_array(0), n_array(0), texcoord_array(0)
{
}

Mesh::Mesh(Mesh const &other)
  : type(other.type), vertices(other.vertices),
    v_array(0), n_array(0), texcoord_array(0)
{
}

Mesh::~Mesh() {
  free(v_array);
  free(n_array);
  free(texcoord_array);
}

void Mesh::transform(Matrix const &trans) {
  Matrix normal_trans = trans.inverse().transpose();

  for (int i = 0 ; i < vertices.count ; i++) {
    vertices.items[i].loc = trans * vertices.items[i].loc;
    vertices.items[i].normal = normal_trans * vertices.items[i].normal;
  }
}

int Mesh::vertexCount() {
  return vertices.count;
}

GLfloat* Mesh::vertexArray() {
  if (!v_array) v_array = (GLfloat*)malloc(sizeof(GLfloat) * vertexCount() * 3);

  int a_pos = 0;
  for (int i = 0 ; i < vertices.count ; i++) {
    v_array[a_pos] = vertices.items[i].loc.x;
    v_array[a_pos + 1] = vertices.items[i].loc.y;
    v_array[a_pos + 2] = vertices.items[i].loc.z;
    a_pos += 3;
  }
  return v_array;
}

GLfloat* Mesh::normalArray() {
  if (!n_array) n_array = (GLfloat*)malloc(sizeof(GLfloat) * vertexCount() * 3);

  int a_pos = 0;
  for (int i = 0 ; i < vertices.count ; i++) {
    n_array[a_pos] = vertices.items[i].normal.x;
    n_array[a_pos + 1] = vertices.items[i].normal.y;
    n_array[a_pos + 2] = vertices.items[i].normal.z;
    a_pos += 3;
  }
  return n_array;
}

GLfloat* Mesh::texcoordArray() {
  if (!texcoord_array) {
    if (vertices.count == 0)
      return 0;
    texcoord_array = (GLfloat*)malloc(sizeof(GLfloat) * vertexCount() * 2);
  }

  int a_pos = 0;
  for (int i = 0 ; i < vertices.count ; i++) {
    texcoord_array[a_pos] = vertices.items[i].texcoord.x;
    texcoord_array[a_pos + 1] = vertices.items[i].texcoord.y;
    a_pos += 2;
  }
  return texcoord_array;
}
/*
Mesh Mesh::createEmpty() {
  Mesh m;
  m.type = TRIANGLES;
  return m;
}
*/
Mesh Mesh::createSquare() {
  Mesh m;
  m.type = TRIANGLE_STRIP;

  Point normal(0, 0, 1);
  m.vertices.addVertex(Vertex(Point(0, 0), normal, Point(0, 0)));
  m.vertices.addVertex(Vertex(Point(0, 1), normal, Point(0, 1)));
  m.vertices.addVertex(Vertex(Point(1, 0), normal, Point(1, 0)));
  m.vertices.addVertex(Vertex(Point(1, 1), normal, Point(1, 1)));

  return m;
}

/**
 * Luo sylinterin ilman päätyjä. Toinen pää on origossa, toinen pisteessä
 * (0, length, 0)
 */
/*
Mesh Mesh::createCylinder(double const length, double const radius,
			  int const slices)
{
  Mesh m;
  m.type = TRIANGLE_STRIP;

  for (int i = 0 ; i <= slices ; i++) {
    double angle = (2 * M_PI / slices) * i;
    double x = cos(angle) * radius;
    double z = sin(angle) * radius;

    Point normal(x, 0.0, z);
    normal.normalize();

    Vertex a(Point(x, 0.0, z), normal);
    m.vertices.addVertex(a);

    Vertex b(Point(x, length, z), normal);
    m.vertices.addVertex(b);
  }

  return m;
}
*/
/**
 * Luo ympyrän, jonka keskipiste on origossa. Vastapäivään kiertävän
 * ympyrän normaali on positiivisen y-akselin suuntaan, myötäpäivään
 * kiertävän negatiivisen y-akselin suuntaan.
 */
/*
Mesh Mesh::createDisk(double const radius, int const slices,
		      Direction const dir)
{
  Mesh m;
  m.type = TRIANGLE_FAN;

  Point normal(0.0, 1.0, 0.0);
  if (dir == CW) normal.y = -1.0;

  Vertex center(Point(0.0, 0.0, 0.0), normal);
  m.vertices.addVertex(center);

  double mult = 1.0;
  if (dir == CCW) mult = -1.0;

  for (int i = 0 ; i <= slices ; i++) {
    double angle = (2 * M_PI / slices) * i;
    double x = cos(angle) * radius;
    double z = sin(angle) * radius * mult;

    Vertex a(Point(x, 0.0, z), normal);
    m.vertices.addVertex(a);
  }

  return m;
}
*/
/*
Mesh Mesh::createFoldedDisk(double const radius, int const slices)
{
  Mesh m;
  m.type = TRIANGLE_FAN;

  Point normal(0.0, 0.0, 1.0);

  Vertex center(Point(0.0, 0.0, 0.0), normal, Point(0.0, 0.0));
  m.vertices.addVertex(center);
    
  for (int i = 0 ; i <= slices * 2; i++) {
    double scale = (i % 2) ? sqrt(2): 1.0;
    double angle = (2 * M_PI / (slices * 2)) * i;
    double x = cos(angle) * radius * scale;
    double y = sin(angle) * radius * scale;

    Vertex a(Point(x, y, 0.0), normal);
    if (i % 2) {
      a.texcoord = Point(1.0, 1.0);
    }
    else if (i % 4)
      a.texcoord = Point(1.0, 0.0);
    else
      a.texcoord = Point(0.0, 1.0);
    m.vertices.addVertex(a);
  }

  return m;
}
*/
/**
 * Luo kartion ilman päätyjä. Toinen pää on origossa, toinen pisteessä
 * (0, length, 0)
 */
Mesh Mesh::createCone(double const length, double const radius1,
		      double const radius2, int const slices) {
  Mesh m;
  m.type = TRIANGLE_STRIP;

  for (int i = 0 ; i <= slices ; i++) {
    double angle = (2 * M_PI / slices) * i;
    double x1 = cos(angle) * radius1;
    double z1 = sin(angle) * radius1;
    double x2 = cos(angle) * radius2;
    double z2 = sin(angle) * radius2;

    Point normal(x1, (x1*x1 - x1*x2 + z1*z1 - z1*z2)/ length, z1);
    normal.normalize();

    Vertex a(Point(x1, 0.0, z1), normal);
    m.vertices.addVertex(a);

    Vertex b(Point(x2, length, z2), normal);
    m.vertices.addVertex(b);
  }

  return m;
}
/*
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
}
*/
Mesh Mesh::createBall(double const radius, int const subdivisions) {
  Mesh m;
  m.type = TRIANGLES;
  /*
  if (subdivisions < 0) {
    error("Subdivisions < 0");
  }
  */
  //if (subdivisions == 0) {
    double length = radius / sqrt(2);
    /*
    Point tc_a(0.0, M_PI / 2.0);
    Point tc_b(-M_PI + M_PI / 4, 0.0);
    Point tc_c(-M_PI / 4, 0.0);
    */
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
      m.vertices.addVertex(va);
      Vertex vb = Vertex(b, normal);
      m.vertices.addVertex(vb);
      Vertex vc = Vertex(c, normal);
      m.vertices.addVertex(vc);
      //tc_b += step;
      //tc_c += step;
    }

    /*
    tc_a.y = -M_PI / 2.0;
    tc_b.x = -M_PI / 4.0;
    tc_c.x = -M_PI + M_PI / 4;
    */
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
      m.vertices.addVertex(va);
      Vertex vb = Vertex(b, normal);
      m.vertices.addVertex(vb);
      Vertex vc = Vertex(c, normal);
      m.vertices.addVertex(vc);
      //tc_b += step;
      //tc_c += step;
    }
    //}
  /*
  else {
    Mesh prev = createBall(radius, subdivisions - 1);
    int i = 0;
    while (i < prev.vertices.count) {
      Vertex a = prev.vertices.items[i]; ++i;
      Vertex b = prev.vertices.items[i]; ++i;
      Vertex c = prev.vertices.items[i]; ++i;

      Point mid_ab = ((a.loc + b.loc) / 2.0).at_length(radius);
      Point mid_ac = ((a.loc + c.loc) / 2.0).at_length(radius);
      Point mid_bc = ((b.loc + c.loc) / 2.0).at_length(radius);

      Point tc_a = a.texcoord;
      Point tc_b = b.texcoord;
      Point tc_c = c.texcoord;
      Point tc_ab = tc_mid(a.texcoord, b.texcoord);
      Point tc_ac = tc_mid(a.texcoord, c.texcoord);
      Point tc_bc = tc_mid(b.texcoord, c.texcoord);
      if (singularity(tc_a)) tc_a.x = tc_bc.x;
      if (singularity(tc_b)) tc_b.x = tc_ac.x;
      if (singularity(tc_c)) tc_c.x = tc_ab.x;

      m.vertices.addVertex(Vertex(a.loc, a.loc.at_length(1), tc_a));
      m.vertices.addVertex(Vertex(mid_ab, mid_ab.at_length(1), tc_ab));
      m.vertices.addVertex(Vertex(mid_ac, mid_ac.at_length(1), tc_ac));

      m.vertices.addVertex(Vertex(mid_ab, mid_ab.at_length(1), tc_ab));
      m.vertices.addVertex(Vertex(b.loc, b.loc.at_length(1), tc_b));
      m.vertices.addVertex(Vertex(mid_bc, mid_bc.at_length(1), tc_bc));

      m.vertices.addVertex(Vertex(mid_ab, mid_ab.at_length(1), tc_ab));
      m.vertices.addVertex(Vertex(mid_bc, mid_bc.at_length(1), tc_bc));
      m.vertices.addVertex(Vertex(mid_ac, mid_ac.at_length(1), tc_ac));

      m.vertices.addVertex(Vertex(mid_ac, mid_ac.at_length(1), tc_ac));
      m.vertices.addVertex(Vertex(mid_bc, mid_bc.at_length(1), tc_bc));
      m.vertices.addVertex(Vertex(c.loc, c.loc.at_length(1), tc_c));
    }
  }
  */
  return m;
}

GLuint Mesh::meshType() {
  switch (this->type) {
  case TRIANGLES:
    return GL_TRIANGLES;
  case TRIANGLE_STRIP:
    return GL_TRIANGLE_STRIP;
  case TRIANGLE_FAN:
    return GL_TRIANGLE_FAN;
  }
  return GL_TRIANGLES;
}

void Mesh::append(Mesh const &other) {
  assert(this->type == other.type);

  if (this->type != TRIANGLES) {
    if (this->vertices.count > 0 && other.vertices.count > 0) {
      Vertex v1 = this->vertices.items[this->vertices.count - 1];
      Vertex v2 = other.vertices.items[0];
      this->vertices.addVertex(v1);
      this->vertices.addVertex(v2);
    }
  }

  for (int i = 0 ; i < other.vertices.count ; ++i) {
    this->vertices.addVertex(other.vertices.items[i]);
  }
}

Mesh Mesh::createHeightMap(double const *heights, int width, int height) {
  Mesh m;
  m.type = TRIANGLE_STRIP;

  for (int z = 1 ; z < height - 2 ; ++z) {
    for (int x = 1 ; x < width - 1 ; ++x) {
      for (int step = 0 ; step <= 1 ; ++step) {
	double val_y1 = heights[(z + step) * width + x];
	Point tangent_az = Point(x, heights[(z + 1 + step) * width + x], z + 1 + step) -
	  Point(x, heights[(z - 1 + step) * width + x], z - 1 + step);
	Point tangent_ax = Point(x + 1, heights[(z + step) * width + x + 1], z + step) -
	  Point(x - 1, heights[(z + step) * width + x - 1], z + step);
	Point normal_a = tangent_az.cross(tangent_ax);
	Vertex a(Point(x, val_y1, z + step), normal_a);
	if ((step == 0 && x == 1 && z > 1) ||
	    (step == 1 && x == width - 2 && z < height - 3))
	  m.vertices.addVertex(a);
	m.vertices.addVertex(a);
      }
    }    
  }
  return m;
}
