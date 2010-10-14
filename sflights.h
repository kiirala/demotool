#ifndef DEMOTOOL_SFLIGHTS_H
#define DEMOTOOL_SFLIGHTS_H

#include "mesh.h"
#include "globject.h"

class SfLights : public GLObject {
public:
  SfLights();

  void render(double time);

  // a, b, m, n1, n2, n3
  double r1[6];
  double r2[6];

private:
  GLuint texture, fbo;
};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_SFLIGHTS_H */
