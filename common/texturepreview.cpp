#include <cstdio>
#include <GL/glew.h>

#include "texturepreview.h"
#include "globject.h"
#include "mesh.h"

const char *vertex_shader =
"#version 120\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_position;\n"
"out vec2 texcoord;\n"
"uniform float size;\n"

"void main(void) {\n"
"    texcoord = in_Position.xy;\n"
"    gl_Position = vec4((in_position.xy - vec2(1.0, 0.0)) * vec2(9.0 / 16.0 * size, size) + vec2(1.0, -1.0),"
"                       (gl_DepthRange.near + gl_DepthRange.far) / 2.0,"
"                       1.0);\n"
"}\n"
  ;

const char *fragment_shader =
"#version 120\n"
"const float PI = 3.14159265;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"//precision highp float;\n"

"in  vec2 texcoord;\n"
  //"out vec4 FragColor;\n"
"uniform sampler2D texture;\n"
"uniform float scale;\n"

"void main(void) {\n"
"    gl_FragColor = vec4(texture2D(texture, texcoord).rgb * scale, 1.0);\n"
"}\n"
;

TexturePreview::TexturePreview()
  : GLObject(Mesh::createSquare(),
	     vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD)),
    texture(0), scale(1.0), size(0.5)
{

}

void TexturePreview::render(double /*time*/) {
  logErrors();
  glShadeModel(GL_SMOOTH);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  render_init();
  uniform_i("texture", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  uniform("size", size);
  uniform("scale", scale);
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  logErrors();
}
