#include <cstdio>
#include <GL/glew.h>

#include "caleidoscope.h"
#include "globject.h"
#include "mesh.h"

static const char *vertex_shader =
"#version 150 compatibility\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_position;\n"
"in  vec2 in_texcoord;\n"
"out vec3 normal;\n"
"out vec2 texcoord;\n"

"void main(void) {\n"
"    texcoord = in_texcoord;\n"
"    gl_Position = gl_ModelViewProjectionMatrix * vec4(in_position, 1.0);\n"
"    normal = gl_NormalMatrix * in_position;\n"
"}\n"
;

static const char *fragment_shader =
"#version 150\n"
"const float PI = 3.14159265;\n"
"uniform sampler2D texture;\n"
"uniform float scale;\n"
"uniform float alpha;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec2 texcoord;\n"
"in  vec3 normal;\n"
"out vec4 FragColor;\n"

"void main(void) {\n"
  //"vec4 c = vec4(polarCoord, 0.0, 1.0);\n"
"    vec4 c = vec4(texture2D(texture, texcoord).rgb * scale, alpha);\n"
"    FragColor = c;"
"}\n"
;

Caleidoscope::Caleidoscope()
  : GLObject(Mesh::createFoldedDisk(10, 20),
	     vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD)),
    scale(1), alpha(1)
{

}

void Caleidoscope::render(double time) {
  render_init();
  glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);
  uniform_i("texture", 0);
  glBindTexture(GL_TEXTURE_2D, texture);
  uniform("scale", scale);
  uniform("alpha", alpha);
  glDrawArrays(GL_TRIANGLE_FAN, 0, mesh.vertexCount());
  logErrors();
}
