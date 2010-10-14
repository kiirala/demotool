#include <cstdio>
#include <string>
#include <unordered_map>
#include <GL/glew.h>

#include "globject.h"
#include "mesh.h"
#include "logger.h"

static void printInfoLog(GLhandleARB h) {
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;
  
  glGetObjectParameterivARB(h, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                            &infologLength);
  
  if (infologLength > 0) {
    infoLog = new char[infologLength];
    glGetInfoLogARB(h, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    delete [] infoLog;
  }
}

unsigned int countBits(unsigned int x) {
  x  = x - ((x >> 1) & 0x55555555);
  x  = (x & 0x33333333) + ((x >> 2) & 0x33333333);
  x  = x + (x >> 4);
  x &= 0xF0F0F0F;
  return (x * 0x01010101) >> 24;
}

GLObject::GLObject(Mesh mesh, char const *vert, char const *frag, ShaderParams params) 
  : mesh(mesh), vbo(new GLuint[countBits(params)]), shader_params(params)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(countBits(shader_params), vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertexCount() * sizeof(GLfloat) * 3,
	       mesh.vertexArray(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  printf("superformula: %d triangles\n", mesh.vertexCount() / 3);

  shader_v = glCreateShader(GL_VERTEX_SHADER);
  shader_f = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(shader_v, 1, &vert, 0);
  glShaderSource(shader_f, 1, &frag, 0);

  glCompileShader(shader_v);
  printInfoLog(shader_v);
  glCompileShader(shader_f);
  printInfoLog(shader_f);

  shader_prog = glCreateProgram();
  glAttachShader(shader_prog, shader_v);
  glAttachShader(shader_prog, shader_f);
  glBindAttribLocation(shader_prog, 0, "in_position");
  glLinkProgram(shader_prog);
  printInfoLog(shader_prog);
}

GLObject::~GLObject() {
  glDetachShader(shader_prog, shader_v);
  glDetachShader(shader_prog, shader_f);
  glDeleteProgram(shader_prog);
  glDeleteShader(shader_v);
  glDeleteShader(shader_f);
  glDeleteBuffers(1, vbo);
  glDeleteVertexArrays(1, &vao);
  delete [] vbo;
}

GLint GLObject::get_param_pos(const char *name) {
  /*
  std::unordered_map<std::string,GLint>::iterator loc = shader_parameters.find(name);
  GLint handle;
  if (loc == shader_parameters.end()) {
    handle = glGetUniformLocation(shader_prog, name.c_str());
    if (handle < 0) {
      error("Shader parameter not found");
      return -1;
    }
    shader_parameters[name] = handle;
  }
  else {
    handle = loc->second;
  }
  return handle;
  */
  return glGetUniformLocation(shader_prog, name);
}
