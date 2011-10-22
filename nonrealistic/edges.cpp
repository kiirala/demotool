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
"uniform sampler2D colour;\n"
"uniform sampler2D depth;\n"

"const float distance = 2.0;\n"

"vec3 rgb_to_hsl(vec4 col) {\n"
"  float minval = min(col.r, min(col.g, col.b));\n"
"  float maxval = max(col.r, max(col.g, col.b));\n"
"  float h = 0.0;\n"
"  float s = 0.0;\n"
"  float l = (maxval + minval) / 2.0;\n"
"  if (minval != maxval) {\n"
"    float diff = maxval - minval;\n"
"    if (l < 0.5) s = diff / (maxval + minval);\n"
"    else s = diff / (2.0 - maxval - minval);\n"
"    if (col.r == maxval) h = (col.g - col.b) / diff;\n"
"    else if (col.g == maxval) h = 2.0 + (col.b - col.r) / diff;\n"
"    else h = 4.0 + (col.r - col.g) / diff;\n"
"  }\n"
"  if (h < 0) h += 6.0;\n"
"  return vec3(h, s, l);\n"
"}\n"

"vec4 hsl_to_rgb(vec3 hsl) {\n"
"  float chroma = (1.0 - abs(2.0 * hsl.z - 1.0)) * hsl.y;\n"
"  float x = chroma * (1.0 - abs(mod(hsl.x, 2.0) - 1.0));\n"
"  vec3 rgb;\n"
"  if (hsl.x < 1.0) rgb = vec3(chroma, x, 0.0);\n"
"  else if (hsl.x < 2.0) rgb = vec3(x, chroma, 0.0);\n"
"  else if (hsl.x < 3.0) rgb = vec3(0.0, chroma, x);\n"
"  else if (hsl.x < 4.0) rgb = vec3(0.0, x, chroma);\n"
"  else if (hsl.x < 5.0) rgb = vec3(x, 0.0, chroma);\n"
"  else rgb = vec3(chroma, 0.0, x);\n"
"  float m = hsl.z - 0.5 * chroma;\n"
"  return vec4(rgb + m, 1.0);\n"
"}\n"

"void main(void) {\n"
"    vec2 dx = vec2(1.0 / 1280.0, 0.0) * distance;\n"
"    vec2 dy = vec2(0.0, 1.0 / 720.0) * distance;\n"
"    vec2 dd = vec2(1.0 / 1280.0, 1.0 / 720.0) * distance;\n"
"    vec4 sum = 8.0 * texture2D(depth, texcoord)\n"
"               - texture2D(depth, texcoord - dx)\n"
"               - texture2D(depth, texcoord + dx)\n"
"               - texture2D(depth, texcoord - dy)\n"
"               - texture2D(depth, texcoord + dy)\n"
"               - texture2D(depth, texcoord - dx - dy)\n"
"               - texture2D(depth, texcoord - dx + dy)\n"
"               - texture2D(depth, texcoord + dx - dy)\n"
"               - texture2D(depth, texcoord + dx + dy);\n"
"    vec4 base_col = texture2D(colour, texcoord);\n"
"    vec4 bg;\n"
"    float cutoff = 0.7;\n"
"    if (texcoord.y > cutoff) bg = mix(vec4(171.0/255.0, 198.0/255.0, 210.0/255.0, 1.0), vec4(30.0/255.0, 238.0/255.0, 255.0/255.0, 1.0), (texcoord.y - cutoff) * 2.0);\n"
"    else bg = mix(vec4(41.0/255.0, 43.0/255.0, 246.0/255.0, 1.0), vec4(171.0/255.0, 177.0/255.0, 210.0/255.0, 1.0), (texcoord.y - 0.2) * 2.0);\n"
  // Add sky
"    base_col = mix(bg, base_col, base_col.a);\n"

"    vec3 hsl = rgb_to_hsl(base_col);\n"
  // Lightness quantization
"    hsl.z = floor(hsl.z * 8.0 + 1.0) / 8.0;\n"
  // sepia
  //"    hsl.y = 0.4;\n"
  //"    hsl.x = 0.66;\n"
"    base_col = hsl_to_rgb(hsl);\n"

  // Add lines
"    base_col = mix(base_col, vec4(0.0, 0.0, 0.0, 1.0), clamp(sum.r * 10.0, 0.0, 1.0));\n"
"    gl_FragColor = vec4(base_col.rgb, 1.0);\n"
"}\n"
;

Edges::Edges()
  : GLObject(Mesh::createSquare(),
	     vertex_shader, fragment_shader,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD)),
    colour_texture(0), depth_texture(0)
{

}

void Edges::render(double /*time*/) {
  logErrors();
  glShadeModel(GL_SMOOTH);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  render_init();

  uniform_i("colour", 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colour_texture);

  uniform_i("depth", 1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, depth_texture);

  glDrawArrays(mesh.meshType(), 0, mesh.vertexCount());

  logErrors();
  glActiveTexture(GL_TEXTURE0);
  logErrors();
}
