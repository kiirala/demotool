#include <cstdio>
#include <cassert>
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <GL/glu.h>

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

struct ParamName {
  GLObject::ShaderParams id;
  char const *name;
  int dimension;
};

ParamName param_names[] = {
  {GLObject::POSITION,  "in_position",  3},
  {GLObject::COLOUR,    "in_colour",    3},
  {GLObject::NORMAL,    "in_normal",    3},
  {GLObject::TANGENT,   "in_tangent",   3},
  {GLObject::BITANGENT, "in_bitangent", 3},
  {GLObject::TEXCOORD,  "in_texcoord",  2},
  {GLObject::EMPTY_PARAMS, 0,           0}
};

GLObject::GLObject(Mesh mesh, char const *vert, char const *frag, ShaderParams params) 
  : mesh(mesh), vbo(new GLuint[countBits(params)]), shader_params(params)
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(countBits(shader_params), vbo);

  printf("mesh: %d triangles\n", mesh.vertexCount() / 3);

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

  assert(shader_params == params);
  int index = 0;
  ParamName *n = param_names;
  while (n->id && n->name) {
    if (shader_params & n->id) {
      assert((unsigned int)index < countBits(shader_params));
      // bind shader attribute to index
      glBindAttribLocation(shader_prog, index, n->name);
      // bind suitable data from mesh to same index
      glBindBuffer(GL_ARRAY_BUFFER, vbo[index]);
      GLfloat *data = 0;
      bool dummy_data = false;
      switch(n->id) {
      case GLObject::POSITION:  data = mesh.vertexArray();   break;
      case GLObject::COLOUR:    data = mesh.colourArray();   break;
      case GLObject::NORMAL:    data = mesh.normalArray();   break;
      case GLObject::TEXCOORD:  data = mesh.texcoordArray(); break;
      default:
	break;
      }
      if (!data) {
	data = new GLfloat[mesh.vertexCount() * n->dimension];
	dummy_data = true;
	error("Unimplemented/missing data %s (0x%x)\n", n->name, n->id);
      }
      logErrors();
      glBufferData(GL_ARRAY_BUFFER,
		   mesh.vertexCount() * sizeof(GLfloat) * n->dimension,
		   data, GL_STATIC_DRAW);
      logErrors();
      glVertexAttribPointer(index, n->dimension, GL_FLOAT, GL_FALSE, 0, 0);
      logErrors();
      glEnableVertexAttribArray(index);
      if (dummy_data) delete [] data;
      ++index;
      logErrors();
      assert(shader_params == params);
    }
    ++n;
  }

  logErrors();
  glLinkProgram(shader_prog);
  printInfoLog(shader_prog);
  logErrors();
}

void GLObject::render_init() {
  logErrors();
  glBindVertexArray(vao);
  logErrors();
  glUseProgram(shader_prog);
  logErrors();
}

GLObject::~GLObject() {
  glDetachShader(shader_prog, shader_v);
  glDetachShader(shader_prog, shader_f);
  glDeleteProgram(shader_prog);
  glDeleteShader(shader_v);
  glDeleteShader(shader_f);
  glDeleteBuffers(countBits(shader_params), vbo);
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

void GLObject::logErrorsImpl(char const *file, int const line) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    fprintf(stderr, "GL error %d at %s:%d: %s\n",
	    err, file, line, gluErrorString(err));
  }
}
