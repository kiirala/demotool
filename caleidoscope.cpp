#include <cstdio>
#include <GL/glew.h>

#include "caleidoscope.h"
#include "globject.h"
#include "mesh.h"

Caleidoscope::Caleidoscope()
  : GLObject(Mesh::createFoldedCircle(10, 10),
	     vertex_shader, fragment_shader,
	     GLObject::POSITION | GLObject::TEXCOORD)
{

}

Caleidoscope::render(double time) {

}
