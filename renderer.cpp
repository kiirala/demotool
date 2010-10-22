#include <stdio.h>
#include <GL/glew.h>

#include "superformula.h"
#include "texturepreview.h"
#include "caleidoscope.h"
#include "interpolate.h"
#include "image.h"
#include "sunrise.h"

#include "img/space.h"
#include "img/isfun.h"
#include "img/ishuge.h"
#include "img/omgponies.h"
#include "img/aurinko.h"
#include "img/pony.h"

extern "C" void load();
extern "C" void unload();
extern "C" void render(double time);
extern "C" void resize(int width, int height);

static void selectRenderTexture(int num);

int const aspectratio_w = 16;
int const aspectratio_h = 9;

GLuint *texture;
const int texture_count = 1;
GLuint fbo;
int viewport[4];

static const Keyframe1 sf_shape[] = {
  {0.0, 2.0},
  {1.0, 4.0},
  {2.0, 6.0},
  {3.0, 8.0},
  {4.0, 10.0},
  {5.0, 12.0},
  {6.0, 6.0},
  {7.0, 10.0},
  {-1.0, 0.0}
};

static const Keyframe1 text_enter[] = {
  {0.0, -1.0},
  {1.0, 0.0},
  {6.0, 0.0},
  {8.0, 4.0},
  {-1.0, 0.0}
};

static const Keyframe<4> sf_rotate[] = {
  {0.0, {50.0, 0.0, 1.0, 0.0}},
  {2.0, {150.0, 1.0, 0.5, 0.0}},
  {4.0, {240.0, 0.0, 1.0, 1.0}},
  {-1, {0, 0, 0, 0}}
};

static const Keyframe<12> sf_begin[] = {
  {0.0, {1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2}},
  {12.0, {1, 1, 1, 1, 2, 2, 1, 1, 1, 1, 2, 2}},
  {13.0, {1.3, 1.0, 12.0, 2.0, 4.0, 4.0, 1.0, 1.0, 2.0, 2.0, 4.0, 4.0}},
  {-1.0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
};

static const Keyframe<4> sun_movement[] = {
  {0.0, {3.0, 2.0, 1.0, 1.0}},
  {12.0, {-1.0, 1.0, 2.0, 1.0}},
  {13.0, {-1.0, 1.0, 2.0, 3.0}},
  {16.0, {-1.0, 1.0, 2.0, 3.0}},
  {17.0, {-1.0, -5.0, -1.0, 3.0}},
  {18.0, {0.0, 0.0, -1.0, 1.0}},
  {-1.0, {0, 0, 0, 0}}
};

static const Keyframe1 drop_sf[] = {
  {0.0, 0.0},
  {33.0, 0.0},
  {34.0, -6.0},
  {34.001, 6.0},
  {35.0, 0.0},
  {47.0, 0.0},
  {48.0, -6.0},
  {48.001, 6.0},
  {49.0, 0.0},
  {-1.0, 0.0}
};

static const Keyframe<3> pony_fly[] = {
  {0.0, {-700, 500, 0}},
  {0.01, {-700, 500, 0}},
  {2.0, {600, 300, 0}},
  {4.0, {650, 270, 0}},
  {8.0, {1800, 100, 0}},
  {-1.0, {0, 0, 0}}
};

class Scene {
public:
  int screen_width, screen_height;

  Superformula sf, sf_lores;
  SfTexture sft;
  SfLights sfl;
  Caleidoscope cal;
  TexturePreview prev;
  Interpolate1 sf_interp, text_i, drop_sf_i;
  Interpolate<4> sf_rotate_i, sun_movement_i;
  Interpolate<12> sf_begin_i;
  Interpolate<3> pony_fly_i;
  Sunrise sunrise;

  Image space, isfun, ishuge, omgponies, aurinko, pony;

  Scene()
    : sf_lores(4),
      sf_interp(sf_shape, 0.0), text_i(text_enter, 16.0),
      drop_sf_i(drop_sf, 0.0),
      sf_rotate_i(sf_rotate, 0.0), sun_movement_i(sun_movement, 0.0),
      sf_begin_i(sf_begin, 0.0), pony_fly_i(pony_fly, 50.0),
      space(drawsvg_space),
      isfun(drawsvg_isfun),
      ishuge(drawsvg_ishuge),
      omgponies(drawsvg_omgponies),
      aurinko(drawsvg_aurinko),
      pony(drawsvg_pony)
  {
    sf_interp.set_loop(8.0);
    sf_rotate_i.set_loop(8.0);
    sf_rotate_i.set_smoothness(1.0);
    sun_movement_i.set_smoothness(0.5);
    pony_fly_i.set_smoothness(1.0);
  }

  void render(double time) {
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    normal_projection();
    glScalef (4., 4., 4.);
    selectRenderTexture(1);
    if (time >= 16) {
      sft.r1[0] = 1.1 + pow(fabs(1.0 - (time - ((int)time)) * 2), 6) * 0.4;
      sft.r2[2] = sf_interp.value(time);
    }
    else {
      sft.set_factors(sf_begin_i.value(time));
    }
    sft.scale = 1.0;
    sft.render(time);
    selectRenderTexture(0);
    if (time >= 16) {
      cal.scale = fmin(0.25, (time - 16) / 4.0);
      cal.alpha = cal.scale * 4.0;
      cal.texture = texture[0];
      cal.render(time);
    }
    int array_count = 1;
    Superformula *use_sf = &sf;
    if (time >= 34 && time < 48) {
      array_count = 7;
      use_sf = &sf_lores;
    }
    for (int x = 0 ; x < array_count ; ++x) {
      for (int y = 0 ; y < array_count ; ++y) {
	glPushMatrix();
	float drop = drop_sf_i.value(time);
	glTranslatef(1.2 * (x - array_count / 2), 1.2 * (y - array_count / 2) + drop, 0);
	double scale = 1.0 / array_count;
	glScalef(scale, scale, scale);
	float *rotate = sf_rotate_i.value(time);
	float div = sqrt(rotate[1] * rotate[1] + rotate[2] * rotate[2] + rotate[3] * rotate[3]);
	glRotatef(rotate[0], rotate[1] / div, rotate[2] / div, rotate[3] / div);
	//if (time < 17) {
	float *v = sun_movement_i.value(time);
	use_sf->lightdir[0] = -v[0];
	use_sf->lightdir[1] = -v[1];
	use_sf->lightdir[2] = v[2];
	//}
	use_sf->heightfield = texture[0];
	use_sf->render(time);
	if (time > 16 && array_count == 1) {
	  sfl.heightfield = texture[0];
	  sfl.render(time);
	}
	glPopMatrix();
      }
    }

    if (time < 17) {
      glPushMatrix();
      float *m = sun_movement_i.value(time);
      glTranslatef(m[0], m[1], m[2]);
      glScalef(m[3], m[3], m[3]);
      glRotatef(time * 10, 0.0, 0.0, -1.0);
      sunrise.render(time);
      glPopMatrix();
    }

    if (time >= 16 && time < 32) {
      text_i.set_start(16.0);
      pixel_coordinate_projection();
      glLoadIdentity();
      glTranslatef(text_i.value(time) * 500, 0, 0);
      space.render(time);

      text_i.set_start(17.0);
      glLoadIdentity();
      glTranslatef(text_i.value(time) * -500, 0, 0);
      isfun.render(time);

      for (int i = 0 ; i < 3 ; ++i) {
	if (time - 18 < i || time - 29 > i) continue;
	glLoadIdentity();
	glTranslatef(40, 200 + 170 * i, 0);
	glScalef(1.5, 1.5, 1.5);
	aurinko.render(time);
      }
    }
    else if (time >= 32 && time < 48) {
      text_i.set_start(32.0);
      pixel_coordinate_projection();
      glLoadIdentity();
      glTranslatef(text_i.value(time) * 500, 0, 0);
      space.render(time);

      text_i.set_start(33.0);
      glLoadIdentity();
      glTranslatef(text_i.value(time) * -500, 0, 0);
      ishuge.render(time);
    }
    else if (time >= 48 && time < 64) {
      text_i.set_start(48.0);
      pixel_coordinate_projection();
      glLoadIdentity();
      glTranslatef(text_i.value(time) * 500, 0, 0);
      space.render(time);

      text_i.set_start(49.0);
      glLoadIdentity();
      glTranslatef(text_i.value(time) * -1100, 0, 0);
      omgponies.render(time);

      for (int i = 0 ; i < 3 ; ++i) {
	float *trans = pony_fly_i.value(time - i * 2);
	glLoadIdentity();
	glTranslatef(trans[0] + (-i + 1) * 300, trans[1], trans[2]);
	pony.render(time);
      }
    }

    if (time > 64) {
      

    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void load() {
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    /* Problem: glewInit failed, something is seriously wrong. */
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  scene = new Scene();

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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scene->render(time);

  glPushMatrix();

  //glRotatef (time * ALPHA, 1, 0, 1);
  glScalef(0.3, 0.3, 0.3);
  // glRotatef (ang, 0, 1, 0);
  // glRotatef (ang, 0, 0, 1);

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
