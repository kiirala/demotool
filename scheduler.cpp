#include <GL/gl.h>

#include "scheduler.h"
#include "datastorage.h"
#include "moduleloader.h"

Scheduler::Scheduler(DataStorage &ds, ModuleLoader &ml)
  : data(ds), loader(ml)
{

}

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
#define ALPHA 0.5

static float ang = 30.;

void Scheduler::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
	
  glRotatef (ang, 1, 0, 1);
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

  ang += ALPHA;
}
