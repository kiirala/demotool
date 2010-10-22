#include <GL/glew.h>

#include "superformula.h"
#include "globject.h"
#include "mesh.h"
#include "logger.h"

static const char *vert_superformula =
"#version 150 compatibility\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_position;\n"
"in  vec2 in_texcoord;\n"
"out vec3 ex_Color;\n"
"out vec2 polarCoord;\n"
"out vec3 normal;\n"
"out vec3 worldPos;\n"
"uniform sampler2D heightfield;\n"

"float sf(vec2 coord) {\n"
"    float height = texture2D(heightfield, (coord + vec2(PI, PI/2.0)) / vec2(2.0 * PI, PI)).r;\n"
"    return height;\n"
"}\n"

"vec3 polar(vec2 coord) {\n"
"    return sf(coord) * vec3(cos(coord.x) * cos(coord.y),"
"                            sin(coord.x) * cos(coord.y),"
"                            sin(coord.y));\n"
"}\n"

"void main(void) {\n"
"    polarCoord = in_texcoord;\n"
"    vec3 pos = in_position * sf(polarCoord);\n"
"    float delta = 1.0 / 512.0;\n"
"    vec2 dt = vec2(delta, 0);\n"
"    vec2 dp = vec2(0, delta);\n"
"    vec3 s1 = polar(polarCoord + dt) - polar(polarCoord - dt);\n"
"    vec3 s2 = polar(polarCoord + dp) - polar(polarCoord - dp);\n"
"    normal = gl_NormalMatrix * cross(normalize(s1), normalize(s2));\n"
"    worldPos = (gl_ModelViewMatrix * vec4(pos, 1.0)).xyz;\n"
"    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);\n"
"    ex_Color = vec3(.616, .243, .824);\n"
"}\n"
;

static const char *frag_superformula =
"#version 150\n"
"const float PI = 3.14159265;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec3 ex_Color;\n"
"in  vec2 polarCoord;\n"
"in  vec3 normal;\n"
"in  vec3 worldPos;\n"
"out vec4 FragColor;\n"
"uniform vec3 lightdir;\n"

"void main(void) {\n"
"    vec3 N = normalize(normal);"
"    vec3 V = normalize(worldPos);"
"    vec3 R = reflect(V, N);"
"    //vec3 L = normalize(vec3(gl_LightSource[0].position));\n"
"    vec3 L = normalize(lightdir);"

"    vec3 color = ex_Color;"
"    float shininess = 10.0;"
"    float scale = 2.0;"
"    vec3 ambient = vec3(0.2) * color;"
"    vec3 diffuse = color * max(dot(-L, N), 0.0);"
"    vec3 specular = vec3(scale) * pow(max(dot(R, L), 0.0), shininess);"

  //"gl_FragColor = vec4((polarCoord + vec2(PI, PI / 2.0)) / vec2(2.0 * PI, PI), 0.0, 1.0);\n"
  //"        gl_FragColor = vec4((N + vec3(0.5)) / 2, 1.0);"
"    FragColor = vec4(ambient + diffuse + specular, 1.0);"
"}\n"
;

static const char *vert_sftex = 
"#version 150\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_position;\n"
"in  vec2 in_texcoord;\n"
"out vec2 polarCoord;\n"
"void main() {\n"
"    polarCoord = in_texcoord * vec2(2.0 * PI, PI) - vec2(PI, PI/2.0);\n"
"    gl_Position = vec4(in_position.xy * 2.0 - vec2(1.0, 1.0), gl_DepthRange.near, 1.0);\n"
"}\n"
;

static const char *frag_sftex =
"#version 150\n"
"const float PI = 3.14159265;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec2 polarCoord;\n"
"out vec4 fragColor;\n"
"uniform vec3 abm1, n1, abm2, n2;\n"
"uniform float scale;\n"

"float r(float phi, vec3 abm, vec3 n) {\n"
"    float mp = abm.z * phi / 4.0;\n"
"    return pow(pow(abs(cos(mp) / abm.x), n.y) + pow(abs(sin(mp) / abm.y), n.z),"
"               -1 / n.x);\n"
"}\n"

"float sf(vec2 coord) {\n"
"    float r1 = r(coord.x, abm1, n1);\n"
"    float r2 = r(coord.y, abm2, n2);\n"
"    vec2 f = vec2(r1 * cos(coord.y), sin(coord.y));\n"
"    return length(f) * r2;\n"
"}\n"

"void main() {\n"
"    float c = sf(polarCoord);\n"
"    fragColor = vec4(c * scale, 0.0, 0.0, 1.0);\n"
"}\n"
;

static const char *vert_sflights =
"#version 150 compatibility\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_position;\n"
"in  vec2 in_texcoord;\n"
"out vec2 polarCoord;\n"
"out vec3 normal;\n"
"uniform float size;\n"

"void main(void) {\n"
  /*
"    float theta;\n"
"    if (in_position.x == 0)\n"
"        theta = sign(in_position.y) * PI / 2.0;\n"
"    else\n"
"        theta = atan(in_position.y, in_position.x);\n"
"    float phi = asin(in_position.z);\n"
"    polarCoord = vec2(theta, phi);\n"
  */
"    polarCoord = in_texcoord;\n"
"    gl_Position = gl_ModelViewProjectionMatrix * vec4(in_position * size, 1.0);\n"
"    normal = gl_NormalMatrix * in_position;\n"
"}\n"
;

static const char *frag_sflights =
"#version 150\n"
"const float PI = 3.14159265;\n"
"uniform float alpha;\n"
"uniform sampler2D heightfield;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec2 polarCoord;\n"
"in  vec3 normal;\n"
"out vec4 FragColor;\n"

"float sf(vec2 coord) {\n"
"    float height = texture2D(heightfield, (coord + vec2(PI, PI / 2.0)) / vec2(2.0 * PI, PI)).r;\n"
"    return height;\n"
"}\n"

"void main(void) {\n"
  //"vec4 c = vec4(polarCoord, 0.0, 1.0);\n"
"    vec4 c = vec4(0.0, 0.9, 1.0, 1.0 - min(1.0, max(0.0, (sf(polarCoord) - 1.3) * 2.0)));\n"
"    c.a *= alpha * pow(abs(dot(normal, vec3(0.0, 0.0, 1.0))), 2.0) * 6.0;\n"
"    FragColor = c;"
"}\n"
;

Superformula::Superformula(int resolution) 
  : GLObject(Mesh::createBall(1, resolution),
	     vert_superformula, frag_superformula,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD)),
    lightdir({0.0, 0.0, -1.0})
{

}

void Superformula::render(double time) {
  render_init();
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glShadeModel(GL_SMOOTH);
  uniform_i("heightfield", 0);
  glBindTexture(GL_TEXTURE_2D, heightfield);
  uniform3("lightdir", lightdir);
  /*
  uniform("abm1", 1.3, 1.0, time);
  uniform("n1", 2.0, 4.0, 4.0);
  uniform("abm2", 1.0, 1.0, 2.0);
  uniform("n2", 2.0, 4.0, 4.0);
  */
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  glDisable(GL_CULL_FACE);
  logErrors();
}

SfTexture::SfTexture()
  : GLObject(Mesh::createSquare(), vert_sftex, frag_sftex,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD)),
    r1({1.3, 1.0, 12.0, 2.0, 4.0, 4.0}),
    r2({1.0, 1.0, 2.0, 2.0, 4.0, 4.0}),
    scale(1.0)
{

}

void SfTexture::render(double time) {
  render_init();
  glDisable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);
  //r1[2] = time;
  uniform3("abm1", r1);
  uniform3("n1", r1 + 3);
  uniform3("abm2", r2);
  uniform3("n2", r2 + 3);
  uniform("scale", scale);
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  logErrors();
}

void SfTexture::set_factors(float *f) {
  for (int i = 0 ; i < 6 ; ++i)
    r1[i] = f[i];
  for (int i = 0 ; i < 6 ; ++i)
    r2[i] = f[i + 6];
}

SfLights::SfLights() 
  : GLObject(Mesh::createBall(1, 4), vert_sflights, frag_sflights,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::TEXCOORD))
{

}

void SfLights::render(double time) {
  render_init();
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  logErrors();
  glShadeModel(GL_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  logErrors();
  glBindVertexArray(vao);
  glUseProgram(shader_prog);
  logErrors();
  uniform_i("heightfield", 0);
  glBindTexture(GL_TEXTURE_2D, heightfield);
  logErrors();
  for (int i = 0 ; i < 5 ; ++i) {
    uniform("size", i + 2);
    uniform("alpha", 1.0 / (i * 2 + 1));
    glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
  }
  glDepthMask(GL_TRUE);
  logErrors();
}
