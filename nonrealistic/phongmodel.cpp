#include <GL/glew.h>

#include "phongmodel.h"
#include "globject.h"
#include "mesh.h"
#include "logger.h"

static const char *vert_phong = 
  //"#version 140 compatibility\n"
"#version 120\n"
  //"in  vec3 in_position;\n"
  //"in  vec3 in_normal;\n"
  //"out vec3 normal, light, reflection;\n"
"attribute vec3 in_position;\n"
"attribute vec3 in_normal;\n"
"varying vec3 v_normal, v_light, v_reflection;\n"

"void main() {\n"
"    vec4 pos_eye = gl_ModelViewMatrix * vec4(in_position, 1.0);\n"
"    gl_Position = gl_ProjectionMatrix * pos_eye;\n"
"    v_normal = gl_NormalMatrix * in_normal;\n"
"    v_light = normalize(pos_eye.xyz - gl_LightSource[0].position.xyz);\n"
"    v_reflection = reflect(vec3(pos_eye), v_normal);\n"
"}\n"
  ;

static const char *frag_phong =
  //"#version 140 compatibility\n"
"#version 120\n"
"uniform vec3 colour;\n"
"uniform float ambient, diffuse, specular, shininess;\n"
  //"in  vec3 normal, light, reflection;\n"
"varying  vec3 v_normal, v_light, v_reflection;\n"
  //"out vec4 out_colour;\n"

"void main() {\n"
"    vec3 normal = normalize(v_normal);\n"
"    vec3 light = normalize(v_light);\n"
"    vec3 reflection = normalize(v_reflection);\n"
"    vec3 ambi_c = colour * ambient;\n"
"    vec3 diff_c = colour * diffuse * max(dot(-light, normal), 0.0);\n"
"    vec3 spec_c = colour * specular * pow(max(dot(-light, reflection), 0.0), shininess);\n"
  //"    out_colour = vec4(ambi_c + diff_c + spec_c, 1.0);\n"
"    gl_FragColor = vec4(ambi_c + diff_c + spec_c, 1.0);\n"
"}\n"
  ;

PhongModel::PhongModel(Mesh mesh, Colour colour,
		       double ambient, double diffuse, double specular,
		       double shininess)
  : GLObject(mesh, vert_phong, frag_phong,
	     (GLObject::ShaderParams)(GLObject::POSITION | GLObject::NORMAL)),
    colour(colour),
    ambient(ambient), diffuse(diffuse), specular(specular),
    shininess(shininess)
{

}

void PhongModel::render(double /*time*/) {
  render_init();
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  uniform("ambient", ambient);
  uniform("diffuse", diffuse);
  uniform("specular", specular);
  uniform("shininess", shininess);
  uniform("colour", colour);

  glDrawArrays(mesh.meshType(), 0, mesh.vertexCount());

  logErrors();
}
