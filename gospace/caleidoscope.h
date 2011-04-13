#ifndef DEMOTOOL_CALEIDOSCOPE_H
#define DEMOTOOL_CALEIDOSCOPE_H

#include "mesh.h"
#include "globject.h"

class Caleidoscope : public GLObject {
public:
  Caleidoscope();

  void render(double time);

  GLuint texture;
  float scale, alpha;

private:

};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_CALEIDOSCOPE_H */
