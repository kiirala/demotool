#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>

#include "logger.h"
#include "glsupport.h"

#include "phongmodel.h"

extern "C" void load();
extern "C" void unload();
extern "C" void render(double time);
extern "C" void resize(int width, int height);

static void selectRenderTexture(int num);

int const aspectratio_w = 16;
int const aspectratio_h = 9;

GLuint *texture;
const GLint texture_format[] = {GL_RGBA, GL_DEPTH_COMPONENT};
const int texture_count = 2;
GLuint fbo;
int viewport[4];

class Scene {
public:
  int screen_width, screen_height;

  PhongModel bunny;

  Scene()
    : bunny(Mesh::loadPly("bun_zipper.ply"),
	    Colour(228.0/255, 147/255.0, 73/255.0),
	    0.1, 0.7, 0.3, 8.0)
  {
  }

  void render(double time) {
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    logErrors();
    normal_projection();
    logErrors();
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    double scale = 100;
    glScaled(scale, scale, scale);
    glTranslated(0, -0.1, 0);

    float pos[] = {1, 1, 2, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glRotated(time * 3, 0, 1, 0);
    bunny.render(time);
    logErrors();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  void normal_projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (-aspectratio_w, aspectratio_w,
	     -aspectratio_h, aspectratio_h,
	     -20050, 10000);

    glMatrixMode(GL_MODELVIEW);    
    glLoadIdentity();
  }

  void pixel_coordinate_projection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1280, 720, 0, -10, 10);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
};
Scene *scene;

static void defaultViewport() {
  logGL();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  logGL();
}

void load() {
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  if (!use_framebuffer_object()) {
    error("Framebuffer objects are required but not supported by OpenGL implementation.\n");
    exit(1);
  }

  if (!use_shader_programs()) {
    error("Shader programs are required but not supported by OpenGL implementation.");
    exit(1);
  }

  if (!use_vertex_buffer_object()) {
    error("Vertex buffer objects are required but not supported by OpenGL implementation");
    exit(1);
  }

  scene = new Scene();

  texture = new GLuint[texture_count];
  glGenTextures(texture_count, texture);
  logGL();
  for (int i = 0 ; i < texture_count ; ++i) {
    glBindTexture(GL_TEXTURE_2D, texture[i]);
    glTexParameterf(GL_TEXTURE_2D,
		    GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D,
		    GL_TEXTURE_MIN_FILTER, GL_LINEAR/*_MIPMAP_LINEAR*/);
    glTexParameterf(GL_TEXTURE_2D,
		    GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,
		    GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D,
    //		    GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 512, 0,
    //		 GL_RGBA, GL_UNSIGNED_BYTE, 0);
    logGL();
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format[i], 512, 512, 0,
		 texture_format[i], GL_UNSIGNED_BYTE, 0);
    logGL();
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			 GL_TEXTURE_2D, texture[0], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
  			 GL_TEXTURE_2D, texture[1], 0);
  logGL();

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(status != GL_FRAMEBUFFER_COMPLETE) {
    if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
      error("FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
    else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
      error("FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
    else if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
      error("FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
    else if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
      error("FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
    else if (status == GL_FRAMEBUFFER_UNSUPPORTED)
      error("FRAMEBUFFER_UNSUPPORTED");
    else if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
      error("FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
    else if (status == GL_FRAMEBUFFER_UNDEFINED)
      error("FRAMEBUFFER_UNDEFINED");
    else
      error("Failed to initialize FBO for unknown reasons.");
  }

  glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
  logGL();
}

void unload() {
  delete scene;
  glDeleteFramebuffers(1, &fbo);
  glDeleteTextures(texture_count, texture);
  delete [] texture;
}

static void selectRenderTexture(int num) {
  if (num > 0 && num <= texture_count) {
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			   GL_TEXTURE_2D, texture[num - 1], 0);
    glViewport(0, 0, 512, 512);
  }
  else {
    glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
    defaultViewport();
  }
}

void render(double time) {
  logErrors();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scene->render(time);
  logErrors();

  glPushMatrix();

  //glRotatef (time * ALPHA, 1, 0, 1);
  glScalef(0.3, 0.3, 0.3);
  // glRotatef (ang, 0, 1, 0);
  // glRotatef (ang, 0, 0, 1);

  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glUseProgram(0);

  glBegin (GL_LINES);
  glColor3f (1., 0., 0.);
  glVertex3f (0., 0., 0.);
  glVertex3f (1., 0., 0.);
  glEnd ();
	
  glBegin (GL_LINES);
  glColor3f (0., 1., 0.);
  glVertex3f (0., 0., 0.);
  glVertex3f (0., 1., 0.);
  glEnd ();
	
  glBegin (GL_LINES);
  glColor3f (0., 0., 1.);
  glVertex3f (0., 0., 0.);
  glVertex3f (0., 0., 1.);
  glEnd ();

  glPopMatrix ();
  /*
  prev->texture = texture[0];
  prev->scale = 0.25;
  prev->size = 1.0;
  prev->render(time);
  */
  logErrors();
}

void resize(int width, int height) {
  scene->screen_width = width;
  scene->screen_height = height;

  int used_w, used_h;
  if (width * aspectratio_h >= height * aspectratio_w) {
    used_w = height * aspectratio_w / aspectratio_h;
    used_h = height;
  }
  else {
    used_w = width;
    used_h = width * aspectratio_h / aspectratio_w;
  }
  
  viewport[0] = (width - used_w) / 2;
  viewport[1] = (height - used_h) / 2;
  viewport[2] = used_w;
  viewport[3] = used_h;
  defaultViewport();
}
