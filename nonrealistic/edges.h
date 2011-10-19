#ifndef DEMOTOOL_EDGES_H
#define DEMOTOOL_EDGES_H

#include "mesh.h"
#include "globject.h"

class Edges : public GLObject {
public:
  Edges();

  void render(double time);

  GLuint texture;

private:

};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_EDGES_H */
