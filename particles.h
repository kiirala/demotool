#ifndef DEMOTOOL_PARTICLES_H
#define DEMOTOOL_PARTICLES_H

#include "mesh.h"
#include "globject.h"

struct Particle {
  float pos[3];
  float 
};

class Particles : public GLObject {
public:
  Particles();

  void render(double time);

  GLuint texture;

private:

};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_PARTICLES_H */
