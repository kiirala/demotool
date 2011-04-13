#include <vector>

#include <GL/glew.h>
#include <GL/gl.h>

bool use_framebuffer_object() {
  if (GLEW_ARB_framebuffer_object)
    return true;

  if (GLEW_EXT_framebuffer_object) {
    glGenFramebuffers = glGenFramebuffersEXT;
    glBindFramebuffer = glBindFramebufferEXT;
    glFramebufferTexture2D = glFramebufferTexture2DEXT;
    glCheckFramebufferStatus = glCheckFramebufferStatusEXT;
    return true;
  }

  return false;
}

struct Program {
  GLuint vertex, fragment;
};
std::vector<Program> programs;

static void useProgram(GLuint program) {
  if (program == 0) {
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, 0);
  }
  else if (program <= programs.size()) {
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, programs[program - 1].vertex);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, programs[program - 1].fragment);
  }
}

bool use_shader_programs() {
  if (GLEW_VERSION_2_0)
    return true;

  if (GLEW_ARB_fragment_program && GLEW_ARB_vertex_program && GLEW_ARB_shader_objects && GLEW_ARB_vertex_shader) {
    // GL_ARB_vertex_program
    glEnableVertexAttribArray = glEnableVertexAttribArrayARB;
    glVertexAttribPointer = glVertexAttribPointerARB;

    // GL_ARB_vertex_shader
    glBindAttribLocation = glBindAttribLocationARB;

    // GL_ARB_shader_objects
    glAttachShader = glAttachObjectARB;
    glCompileShader = glCompileShaderARB;
    glCreateProgram = glCreateProgramObjectARB;
    glCreateShader = glCreateShaderObjectARB;
    glGetUniformLocation = glGetUniformLocationARB;
    glLinkProgram = glLinkProgramARB;
    glShaderSource = glShaderSourceARB;
    glUniform1i = glUniform1iARB;
    glUniform1f = glUniform1fARB;
    glUniform3f = glUniform3fARB;
    glUniform3fv = glUniform3fvARB;
    glUseProgram = glUseProgramObjectARB;

    return true;
  }

  return false;
}

bool use_vertex_buffer_object() {
  if(GLEW_VERSION_1_5)
    return true;

  if (GLEW_ARB_vertex_buffer_object) {
    glGenBuffers = glGenBuffersARB;
    glBindBuffer = glBindBufferARB;
    glDeleteBuffers = glDeleteBuffersARB;
    glBufferData = glBufferDataARB;
    return true;
  }

  return false;
}
