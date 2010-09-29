#ifndef TT_MESH_H
#define TT_MESH_H

#include <vector>

#include "matrix.h"
#include "types.h"

class Mesh {
public:
  enum Direction {
    CW, CCW
  };

  void setColour(double r, double g, double b) {
    colour.set(r, g, b);
  }

  void render() const;
  void transform(Matrix const &trans);
  int vertexCount();
  GLfloat *vertexArray();

  static Mesh createCylinder(double const length, double const radius,
			     int const slices = 16);
  static Mesh createDisk(double const radius, int const slices = 16,
			 Direction const dir = CCW);
  static Mesh createCone(double const length, double const radius1,
			 double const radius2, int const slices = 16);

  static Mesh createBall(double const radius, int const subdivisions);

  ~Mesh();

private:
  Mesh();
  enum {
    TRIANGLES,
    TRIANGLE_FAN,
    TRIANGLE_STRIP
  } type;
  Colour colour;
  std::vector<Vertex> vertices;
  GLfloat *v_array;
};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TT_MESH_H */
