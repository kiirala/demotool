#include <GL/gl.h>

extern "C" void render(double time);
extern "C" void resize(int width, int height);

float boxv[][3] = {
	{ -0.5, -0.5, -0.5 },
	{  0.5, -0.5, -0.5 },
	{  0.5,  0.5, -0.5 },
	{ -0.5,  0.5, -0.5 },
	{ -0.5, -0.5,  0.5 },
	{  0.5, -0.5,  0.5 },
	{  0.5,  0.5,  0.5 },
	{ -0.5,  0.5,  0.5 }
};
#define ALPHA 90.0

void render(double time) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
	
  glRotatef (time * ALPHA, 1, 0, 1);
  // glRotatef (ang, 0, 1, 0);
  // glRotatef (ang, 0, 0, 1);

  glShadeModel(GL_FLAT);

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

  glBegin(GL_LINES);
  glColor3f (1., 1., 1.);
  glVertex3fv(boxv[0]);
  glVertex3fv(boxv[1]);
	
  glVertex3fv(boxv[1]);
  glVertex3fv(boxv[2]);
	
  glVertex3fv(boxv[2]);
  glVertex3fv(boxv[3]);
	
  glVertex3fv(boxv[3]);
  glVertex3fv(boxv[0]);
	
  glVertex3fv(boxv[4]);
  glVertex3fv(boxv[5]);
	
  glVertex3fv(boxv[5]);
  glVertex3fv(boxv[6]);
	
  glVertex3fv(boxv[6]);
  glVertex3fv(boxv[7]);
	
  glVertex3fv(boxv[7]);
  glVertex3fv(boxv[4]);
	
  glVertex3fv(boxv[0]);
  glVertex3fv(boxv[4]);
	
  glVertex3fv(boxv[1]);
  glVertex3fv(boxv[5]);
	
  glVertex3fv(boxv[2]);
  glVertex3fv(boxv[6]);
	
  glVertex3fv(boxv[3]);
  glVertex3fv(boxv[7]);
  glEnd();

  glPopMatrix ();
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

  glViewport ((width - used_w) / 2, (height - used_h) / 2, used_w, used_h);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho (-aspectratio_w, aspectratio_w,
	   -aspectratio_h, aspectratio_h,
	   -20050, 10000);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glMatrixMode(GL_MODELVIEW);
  
  glLoadIdentity();
  glScalef (10., 10., 10.);
}
