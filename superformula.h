#ifndef DEMOTOOL_SUPERFORMULA_H
#define DEMOTOOL_SUPERFORMULA_H

#include <GL/gl.h>

#include "mesh.h"

class Superformula {
public:
  Superformula();
  ~Superformula();

  void render();

  // a, b, m, n1, n2, n3
  double r1[6];
  double r2[6];

private:
  Mesh mesh;
  GLuint vao, *vbo;
  GLuint shader_v, shader_f;
  GLuint shader_prog;
};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_SUPERFORMULA_H */
