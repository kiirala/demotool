#ifndef DEMOTOOL_SUPERFORMULA_H
#define DEMOTOOL_SUPERFORMULA_H

#include "mesh.h"
#include "globject.h"

class Superformula : public GLObject {
public:
  Superformula(int resolution = 7);

  void render(double time);

  GLuint heightfield;
  float lightdir[3];

private:

};

class SfTexture : public GLObject {
public:
  SfTexture();

  void render(double time);

  void set_factors(float *f);

  // a, b, m, n1, n2, n3
  GLfloat r1[6];
  GLfloat r2[6];

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
