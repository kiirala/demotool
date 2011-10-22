#ifndef TT_MESH_H
#define TT_MESH_H

//#include <vector>
#include <string>

#include "matrix.h"
#include "types.h"

class VertexList {
public:
  Vertex *items;
  int count;
  int capacity;

  VertexList();
  VertexList(VertexList const &other);
  ~VertexList();
  void addVertex(Vertex const &v);
};

class Mesh {
public:
  enum Direction {
    CW, CCW
  };

  Mesh(Mesh const &other);
  
  void append(Mesh const &other);

  void transform(Matrix const &trans);
  int vertexCount();
  GLfloat *vertexArray();
  GLfloat *normalArray();
  GLfloat *texcoordArray();

  GLuint meshType();

  static Mesh createEmpty();
  static Mesh createSquare();
  static Mesh createCylinder(double const length, double const radius,
			     int const slices = 16);
  static Mesh createDisk(double const radius, int const slices = 16,
			 Direction const dir = CCW);
  static Mesh createFoldedDisk(double const radius, int const slices = 16);
  static Mesh createCone(double const length, double const radius1,
			 double const radius2, int const slices = 16);

  static Mesh createBall(double const radius, int const subdivisions);
  static Mesh loadPly(std::string const &filename);
  static Mesh createHeightMap(double const *heights, int width, int height);

  ~Mesh();

private:
  Mesh();
  enum {
    TRIANGLES,
    TRIANGLE_FAN,
    TRIANGLE_STRIP
  } type;
  VertexList vertices;
  GLfloat *v_array;
  GLfloat *n_array;
  GLfloat *texcoord_array;
};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TT_MESH_H */
