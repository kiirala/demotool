#include <stdio.h>
#include <GL/glew.h>

#include "superformula.h"
#include "mesh.h"

static const char *vertex_shader =
"#version 150\n"
"const float PI = 3.14159265;\n"
"in  vec3 in_Position;\n"
"out vec3 ex_Color;\n"
"out vec2 polarCoord;\n"
"out vec3 normal;\n"
"out vec3 worldPos;\n"

"float r(float phi, vec3 abm, vec3 n) {\n"
"    float mp = abm.z * phi / 4.0;\n"
"    return pow(pow(abs(cos(mp) / abm.x), n.y) + pow(abs(sin(mp) / abm.y), n.z),"
"               -1 / n.x);\n"
"}\n"

"vec3 sf(float theta, float phi) {\n"
"    float r1 = r(theta, vec3(1.3, 1.0, 12.0), vec3(2.0, 4.0, 4.0));\n"
"    float r2 = r(phi, vec3(1.0, 1.0, 2.0), vec3(2.0, 4.0, 4.0));\n"
"    return vec3(r1 * cos(theta) * r2 * cos(phi),"
"                r1 * sin(theta) * r2 * cos(phi),"
"                r2 * sin(phi));\n"
"}"

"void main(void) {\n"
"    float theta = atan(in_Position.y, in_Position.x);\n"
"    float phi = asin(in_Position.z);\n"
"    polarCoord = vec2(theta, phi);\n"
"    vec3 pos = sf(theta, phi);\n"
"    vec3 s1 = sf(theta + .01, phi);\n"
"    vec3 s2 = sf(theta, phi + .01);\n"
"    normal = gl_NormalMatrix * normalize(cross(s1 - pos, s2 - pos));\n"
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

"void main(void) {\n"
"        vec3 N = normalize(normal);"
"        vec3 V = normalize(worldPos);"
"        vec3 R = reflect(V, N);"
"        //vec3 L = normalize(vec3(gl_LightSource[0].position));\n"
"        vec3 L = normalize(vec3(0.0, 0.0, -1.0));"

"        vec3 color = ex_Color;"
"        float shininess = 10.0;"
"        float scale = 2.0;"
"        vec3 ambient = vec3(0.2) * color;"
"        vec3 diffuse = color * max(dot(-L, N), 0.0);"
"        vec3 specular = vec3(scale) * pow(max(dot(R, L), 0.0), shininess);"

"        gl_FragColor = vec4(ambient + diffuse + specular, 1.0);"
"}\n"
;

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

Superformula::Superformula() 
  : mesh(Mesh::createBall(1, 8)), vbo(new GLuint[1])
{
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, vbo);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, mesh.vertexCount() * sizeof(GLfloat) * 3,
	       mesh.vertexArray(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);
  printf("superformula: %d triangles\n", mesh.vertexCount() / 3);

  shader_v = glCreateShader(GL_VERTEX_SHADER);
  shader_f = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(shader_v, 1, &vertex_shader, 0);
  glShaderSource(shader_f, 1, &fragment_shader, 0);

  glCompileShader(shader_v);
  printInfoLog(shader_v);
  glCompileShader(shader_f);
  printInfoLog(shader_f);

  shader_prog = glCreateProgram();
  glAttachShader(shader_prog, shader_v);
  glAttachShader(shader_prog, shader_f);
  glBindAttribLocation(shader_prog, 0, "in_position");
  glLinkProgram(shader_prog);
  printInfoLog(shader_prog);
}

Superformula::~Superformula() {
  glDetachShader(shader_prog, shader_v);
  glDetachShader(shader_prog, shader_f);
  glDeleteProgram(shader_prog);
  glDeleteShader(shader_v);
  glDeleteShader(shader_f);
  glDeleteBuffers(1, vbo);
  glDeleteVertexArrays(1, &vao);
  delete [] vbo;
}

void Superformula::render() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glBindVertexArray(vao);
  glUseProgram(shader_prog);
  glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
}
