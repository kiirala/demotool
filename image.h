#ifndef DEMOTOOL_IMAGE_H
#define DEMOTOOL_IMAGE_H

#include "globject.h"

struct BezierPath {
  float f[6];
};

struct BezierShape {
  float colour[3];
  BezierPath *path;
};

class Image : public GLObject {
public:
  Image(BezierShape *shapes);

  void render(double time);

  GLuint texture;

private:
  std::vector<Mesh> shapes;
};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_IMAGE_H */
