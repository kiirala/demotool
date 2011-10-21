#ifndef DEMOTOOL_GLOBJECT_H
#define DEMOTOOL_GLOBJECT_H

#include <string>
#include <unordered_map>
#include <GL/glew.h>

#include "mesh.h"
#include "types.h"

#define logErrors() GLObject::logErrorsImpl(__FILE__, __LINE__)

class GLObject {
public:
  enum ShaderParams {
    EMPTY_PARAMS = 0,
    POSITION = 1,
    COLOUR = 2,
    NORMAL = 4,
    TANGENT = 8,
    BITANGENT = 0x10,
    TEXCOORD = 0x20
  };
  GLObject(Mesh mesh, char const *vert, char const *frag, ShaderParams params);
  ~GLObject();

  virtual void render(double time) = 0;
  void render_init();
  static void logErrorsImpl(char const *file, int const line);

protected:
  Mesh mesh;
  GLuint vao, *vbo;
  GLuint shader_v, shader_f;
  GLuint shader_prog;
  ShaderParams shader_params;
  //std::unordered_map<std::string,GLint> shader_parameters;
  GLint get_param_pos(const char *name);
  void uniform_i(const char *name, GLint val) {
    glUniform1i(get_param_pos(name), val);
  }
  void uniform(const char *name, GLfloat val) {
    glUniform1f(get_param_pos(name), val);
  }
  void uniform(const char *name, GLfloat v1, GLfloat v2, GLfloat v3) {
    glUniform3f(get_param_pos(name), v1, v2, v3);
  }
  void uniform(const char *name, Colour const &c) {
    glUniform3f(get_param_pos(name), c.r, c.g, c.b);
  }
  void uniform3(const char *name, GLfloat *v) {
    glUniform3fv(get_param_pos(name), 1, v);
  }
};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_GLOBJECT_H */
