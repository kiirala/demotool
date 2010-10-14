#include <cstdio>
#include <GL/glew.h>

#include "sflights.h"
#include "globject.h"
#include "mesh.h"
#include "logger.h"

static const char *vertex_shader =
"#version 150 compatibility\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_Position;\n"
"out vec4 ex_Color;\n"
"out vec3 normal;\n"
"uniform vec3 abm1, n1, abm2, n2;\n"
"uniform float size;\n"

"float r(float phi, vec3 abm, vec3 n) {\n"
"    float mp = abm.z * phi / 4.0;\n"
"    return pow(pow(abs(cos(mp) / abm.x), n.y) + pow(abs(sin(mp) / abm.y), n.z),"
"               -1 / n.x);\n"
"}\n"

"vec3 sf(float theta, float phi) {\n"
"    float r1 = r(theta, abm1, n1);\n"
"    float r2 = r(phi, abm2, n2);\n"
"    vec3 r = vec3(r1 * cos(theta) * r2 * cos(phi),"
"                  r1 * sin(theta) * r2 * cos(phi),"
"                  r2 * sin(phi));\n"
"    return r;\n"
"}"

"void main(void) {\n"
"    float theta;\n"
"    if (in_Position.x == 0)\n"
"        theta = sign(in_Position.y) * PI / 2.0;\n"
"    else\n"
"        theta = atan(in_Position.y, in_Position.x);\n"
"    float phi = asin(in_Position.z);\n"
"    vec3 sfpos = sf(theta, phi);\n"
"    gl_Position = gl_ModelViewProjectionMatrix * vec4(in_Position * size, 1.0);\n"
"    normal = gl_NormalMatrix * in_Position;\n"
"    ex_Color = vec4(0.0, 0.9, 1.0, 1.0 - min(1.0, max(0.0, (length(sfpos) - 1.3) * 4.0)) );\n"
"}\n"
;

static const char *fragment_shader =
"#version 150\n"
"const float PI = 3.14159265;\n"
"uniform float alpha;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec4 ex_Color;\n"
"in  vec3 normal;\n"
"out vec4 FragColor;\n"

"void main(void) {\n"
"    vec4 c = ex_Color;\n"
"    c.a *= alpha * pow(abs(dot(normal, vec3(0.0, 0.0, 1.0))), 2.0) * 6.0;\n"
"    FragColor = c;"
"}\n"
;

SfLights::SfLights() 
  : GLObject(Mesh::createBall(1, 7), vertex_shader, fragment_shader,
	     GLObject::POSITION)
{
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D,
		  GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,
		  GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_2D,
		  GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D,
		  GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,
		  GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0,
	       GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER_EXT, fbo);

  glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			    GL_TEXTURE_2D, texture, 0);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("FBO not fully initialized\n");

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void SfLights::render(double time) {
  //glPushAttrib(GL_VIEWPORT_BIT);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindVertexArray(vao);
  glUseProgram(shader_prog);
  uniform("abm1", 1.3, 1.0, time);
  uniform("n1", 2.0, 4.0, 4.0);
  uniform("abm2", 1.0, 1.0, 2.0);
  uniform("n2", 2.0, 4.0, 4.0);
  for (int i = 0 ; i < 5 ; ++i) {
    uniform("size", i + 2);
    uniform("alpha", 1.0 / (i * 2 + 1));
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  }
  //glPopAttrib();
  glDepthMask(GL_TRUE);


}
