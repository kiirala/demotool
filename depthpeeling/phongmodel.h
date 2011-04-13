#ifndef DEMOTOOL_PHONGMODEL_H
#define DEMOTOOL_PHONGMODEL_H

#include "mesh.h"
#include "globject.h"

class PhongModel : public GLObject {
public:
  PhongModel(Mesh mesh, Colour colour,
	     double ambient, double diffuse, double specular, double shininess);

  void render(double time);

  Colour colour;
  double ambient, diffuse, specular, shininess;

private:

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_PHONGMODEL_H */
