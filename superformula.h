#ifndef DEMOTOOL_SUPERFORMULA_H
#define DEMOTOOL_SUPERFORMULA_H

#include "mesh.h"
#include "globject.h"

class Superformula : public GLObject {
public:
  Superformula();

  void render(double time);

  GLuint heightfield;

private:

};

class SfTexture : public GLObject {
public:
  SfTexture();

  void render(double time);

  // a, b, m, n1, n2, n3
  double r1[6];
  double r2[6];

  GLfloat scale;

private:

};

class SfLights : public GLObject {
public:
  SfLights();

  void render(double time);

  GLuint heightfield;

private:

};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_SUPERFORMULA_H */
