#ifndef DEMOTOOL_SUPERFORMULA_H
#define DEMOTOOL_SUPERFORMULA_H

#include <string>
#include <unordered_map>
#include <GL/gl.h>

#include "mesh.h"

class Superformula {
public:
  Superformula();
  ~Superformula();

  void render(double time);

  // a, b, m, n1, n2, n3
  double r1[6];
  double r2[6];

private:
  Mesh mesh;
  GLuint vao, *vbo;
  GLuint shader_v, shader_f;
  GLuint shader_prog;
  //std::unordered_map<std::string,GLint> shader_parameters;
  GLint get_param_pos(const char *name);
  void uniform(const char *name, GLfloat val) {
    glUniform1f(get_param_pos(name), val);
  }
  void uniform(const char *name, GLfloat v1, GLfloat v2, GLfloat v3) {
    glUniform3f(get_param_pos(name), v1, v2, v3);
  }
};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_SUPERFORMULA_H */
