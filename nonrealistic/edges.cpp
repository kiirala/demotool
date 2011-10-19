#include <cstdio>
#include <GL/glew.h>

#include "edges.h"
#include "globject.h"
#include "mesh.h"

const char *vertex_shader =
"#version 120\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_Position;\n"
"out vec2 texcoord;\n"

"void main(void) {\n"
"    texcoord = in_Position.xy;\n"
"    gl_Position = vec4((in_Position.xy - vec2(1.0, 0.0)) * vec2(9.0 / 16.0, 1.0) + vec2(1.0, -1.0),"
"                       (gl_DepthRange.near + gl_DepthRange.far) / 2.0,"
"                       1.0);\n"
"    gl_Position = vec4(in_Position.xy * 2.0 - vec2(1.0, 1.0), (gl_DepthRange.near + gl_DepthRange.far) / 2.0, 1.0);\n"
"}\n"
  ;

const char *fragment_shader =
"#version 120\n"
"const float PI = 3.14159265;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"//precision highp float;\n"

"in  vec2 texcoord;\n"
"uniform sampler2D texture;\n"

"const float distance = 1.0;\n"

"void main(void) {\n"
"    vec2 dx = vec2(1.0 / 1280.0, 0.0) * distance;\n"
"    vec2 dy = vec2(0.0, 1.0 / 720.0) * distance;\n"
"    vec2 dd = vec2(1.0 / 1280.0, 1.0 / 720.0) * distance;\n"
"    vec4 sum = 8.0 * texture2D(texture, texcoord)\n"
"               - texture2D(texture, texcoord - dx)\n"
"               - texture2D(texture, texcoord + dx)\n"
"               - texture2D(texture, texcoord - dy)\n"
"               - texture2D(texture, texcoord + dy)\n"
"               - texture2D(texture, texcoord - dx - dy)\n"
"               - texture2D(texture, texcoord - dx + dy)\n"
"               - texture2D(texture, texcoord + dx - dy)\n"
"               - texture2D(texture, texcoord + dx + dy);\n"
"    gl_FragColor = vec4(vec3(1.0) - sum.rgb * 1.0, 1.0);\n"
"}\n"
;

Edges::Edges()
  : GLObject(Mesh::createSquare(),
	     vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD)),
    texture(0)
{

}

void Edges::render(double /*time*/) {
  logErrors();
  glShadeModel(GL_SMOOTH);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  render_init();
  uniform_i("texture", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  logErrors();
}
