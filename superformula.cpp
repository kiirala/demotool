#include <GL/glew.h>

#include "superformula.h"
#include "globject.h"
#include "mesh.h"
#include "logger.h"

static const char *vertex_shader =
"#version 150 compatibility\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_Position;\n"
"out vec3 ex_Color;\n"
"out vec2 polarCoord;\n"
"out vec3 normal;\n"
"out vec3 worldPos;\n"
"uniform vec3 abm1, n1, abm2, n2;\n"

"float r(float phi, vec3 abm, vec3 n) {\n"
"    float mp = abm.z * phi / 4.0;\n"
"    return pow(pow(abs(cos(mp) / abm.x), n.y) + pow(abs(sin(mp) / abm.y), n.z),"
"               -1 / n.x);\n"
"}\n"

"vec3 sf(float theta, float phi) {\n"
"    float r1 = r(theta, abm1, n1);\n"
"    float r2 = r(phi, abm2, n2);\n"
"    return vec3(r1 * cos(theta) * r2 * cos(phi),"
"                r1 * sin(theta) * r2 * cos(phi),"
"                r2 * sin(phi));\n"
"}"

"void main(void) {\n"
"    float theta;\n"
"    if (in_Position.x == 0)\n"
"        theta = sign(in_Position.y) * PI / 2.0;\n"
"    else\n"
"        theta = atan(in_Position.y, in_Position.x);\n"
"    float phi = asin(in_Position.z);\n"
"    polarCoord = vec2(theta, phi);\n"
"    vec3 pos = sf(theta, phi);\n"
"    float delta = 0.0001;\n"
"    vec3 s1 = sf(theta + delta, phi) - sf(theta - delta, phi);\n"
"    vec3 s2 = sf(theta, phi + delta) - sf(theta, phi - delta);\n"
"    normal = gl_NormalMatrix * cross(normalize(s1), normalize(s2));\n"
"    worldPos = (gl_ModelViewMatrix * vec4(pos, 1.0)).xyz;\n"
"    gl_Position = gl_ModelViewProjectionMatrix * vec4(pos, 1.0);\n"
"    ex_Color = vec3(.616, .243, .824);\n"
"}\n"
;

static const char *fragment_shader =
"#version 150\n"
"const float PI = 3.14159265;\n"

"// It was expressed that some drivers required this next line to function properly\n"
"precision highp float;\n"

"in  vec3 ex_Color;\n"
"in  vec2 polarCoord;\n"
"in  vec3 normal;\n"
"in  vec3 worldPos;\n"
"out vec4 FragColor;\n"

"void main(void) {\n"
"        vec3 N = normalize(normal);"
"        vec3 V = normalize(worldPos);"
"        vec3 R = reflect(V, N);"
"        //vec3 L = normalize(vec3(gl_LightSource[0].position));\n"
"        vec3 L = vec3(0.0, 0.0, -1.0);"

"        vec3 color = ex_Color;"
"        float shininess = 10.0;"
"        float scale = 2.0;"
"        vec3 ambient = vec3(0.2) * color;"
"        vec3 diffuse = color * max(dot(-L, N), 0.0);"
"        vec3 specular = vec3(scale) * pow(max(dot(R, L), 0.0), shininess);"

  //"        gl_FragColor = vec4((N + vec3(0.5)) / 2, 1.0);"
"        FragColor = vec4(ambient + diffuse + specular, 1.0);"
"}\n"
;

Superformula::Superformula() 
  : GLObject(Mesh::createBall(1, 7), vertex_shader, fragment_shader,
	     GLObject::POSITION)
{

}

Superformula::~Superformula() {
}

void Superformula::render(double time) {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);
  glBindVertexArray(vao);
  glUseProgram(shader_prog);
  uniform("abm1", 1.3, 1.0, time);
  uniform("n1", 2.0, 4.0, 4.0);
  uniform("abm2", 1.0, 1.0, 2.0);
  uniform("n2", 2.0, 4.0, 4.0);
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
}
