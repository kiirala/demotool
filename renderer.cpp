#include <stdio.h>
#include <GL/glew.h>

#include "superformula.h"
#include "texturepreview.h"
#include "caleidoscope.h"
#include "interpolate.h"

extern "C" void load();
extern "C" void unload();
extern "C" void render(double time);
extern "C" void resize(int width, int height);

#define ALPHA 90.0

Superformula *sf;
SfTexture *sft;
SfLights *sfl;
Caleidoscope *cal;
TexturePreview *prev;
GLuint *texture;
const int texture_count = 1;
GLuint fbo;
int viewport[4];

Keyframe sf_shape[] = {
  {0.0, 4.0},
  {4.0, 16.0},
  {8.0, 12.0},
  {-1.0, 0.0}
};
Interpolate *sf_interp;

static void defaultViewport() {
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void load() {
  //sf = 0;

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  sf = new Superformula();
  sft = new SfTexture();
  sfl = new SfLights();
  cal = new Caleidoscope();
  prev = new TexturePreview();
  sf_interp = new Interpolate(sf_shape, 0.0);

  texture = new GLuint[texture_count];
  glGenTextures(texture_count, texture);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 512, 512, 0,
		 GL_RGBA, GL_UNSIGNED_BYTE, 0);
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER_EXT, fbo);

  glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
			 GL_TEXTURE_2D, texture[0], 0);

  GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
    printf("FBO not fully initialized\n");

  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

void unload() {
  delete sf;
  delete sft;
  delete sfl;
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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();

  //glRotatef (time * ALPHA, 1, 0, 1);
  glScalef(0.3, 0.3, 0.3);
  // glRotatef (ang, 0, 1, 0);
  // glRotatef (ang, 0, 0, 1);

  //if (!sf) sf = new Superformula();
  //if (!sfl) sfl = new SfLights();

  selectRenderTexture(1);
  sft->scale = 1.0;
  sft->r1[2] = sf_interp->value(time);
  sft->render(time);
  selectRenderTexture(0);
  cal->texture = texture[0];
  cal->render(time);
  //sft->scale = 0.25;
  //sft->render(time);
  sf->heightfield = texture[0];
  sf->render(time);
  sfl->heightfield = texture[0];
  sfl->render(time);

  glShadeModel(GL_FLAT);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
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
}

void resize(int width, int height) {
  int const aspectratio_w = 16;
  int const aspectratio_h = 9;

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

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho (-aspectratio_w, aspectratio_w,
	   -aspectratio_h, aspectratio_h,
	   -20050, 10000);
  glMatrixMode(GL_MODELVIEW);
  
  glLoadIdentity();
  glScalef (10., 10., 10.);
}
