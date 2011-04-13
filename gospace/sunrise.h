#ifndef DEMOTOOL_SUNRISE_H
#define DEMOTOOL_SUNRISE_H

#include "mesh.h"
#include "globject.h"

class Sunrise : public GLObject {
public:
  Sunrise();

  void render(double time);

  GLuint texture;

private:

};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_SUNRISE_H */
