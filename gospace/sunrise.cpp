#include <GL/glew.h>

#include "sunrise.h"
#include "globject.h"
#include "mesh.h"
#include "logger.h"

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

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec2 texcoord;\n"
"in  vec3 normal;\n"
"out vec4 FragColor;\n"

"void main(void) {\n"
"    float sun = max(0.0, pow(1.0 - length(texcoord), 3.0));\n"
"    float ray = max(0.0, pow(1.0 - min(texcoord.x, texcoord.y / 1.5) - max(texcoord.x, texcoord.y * 1.5) / 2.0, 5.0));\n"
"    vec4 c = vec4(1.0, 0.765, 0.267, ray) + sun * vec4(1.0, 1.0, 1.0, 1.0);\n"
"    FragColor = c;"
"}\n"
;

Sunrise::Sunrise()
  : GLObject(Mesh::createFoldedDisk(1, 12),
	     vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD))
{

}

void Sunrise::render(double time) {
  render_init();
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);
  glDrawArrays(GL_TRIANGLE_FAN, 0, mesh.vertexCount());
  logErrors();
}
