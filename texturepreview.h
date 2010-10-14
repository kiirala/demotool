#ifndef DEMOTOOL_TEXTUREPREVIEW_H
#define DEMOTOOL_TEXTUREPREVIEW_H

#include "mesh.h"
#include "globject.h"

class TexturePreview : public GLObject {
public:
  TexturePreview();

  void render(double time);

  GLuint texture;
  GLfloat scale, size;

private:

};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_TEXTUREPREVIEW_H */
