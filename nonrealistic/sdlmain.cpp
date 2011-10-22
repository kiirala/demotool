#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#include <time.h>
#include <unistd.h>

#include <GL/gl.h>

extern "C" void load();
extern "C" void unload();
extern "C" void render(double time);
extern "C" void simpleRender();
extern "C" void resize(int width, int height);

static bool fullscreen = true;
static int scrWidth = 1280;
static int scrHeight = 720;
static const int scrBPP = 32;
//static const char *font_file = "/usr/share/fonts/ttf-bitstream-vera/Vera.ttf";

void init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

  /* Set the video mode */
  int videoFlags = SDL_OPENGL;
  if (fullscreen)
    videoFlags |= SDL_FULLSCREEN;

  SDL_Surface *surface = SDL_SetVideoMode(scrWidth, scrHeight, scrBPP,
					  videoFlags);
  if (!surface) {
    fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  SDL_WM_SetCaption("Nonrealistic rendering", "Nonrealistic rendering");
}

void cleanup(int /*state*/, void * /*foo*/) {
  unload();
  SDL_Quit();
}

int main(int argc, char ** argv) {
  int opt;
  while ((opt = getopt(argc, argv, "fws:")) != -1) {
    switch (opt) {
    case 'f':
      fullscreen = true;
      break;
    case 'w':
      fullscreen = false;
      break;
    case 's': {
      int r = sscanf(optarg, "%dx%d", &scrWidth, &scrHeight);
      if (r == 2)
	break;
      // else fall through
    }
    default: /* '?' */
      fprintf(stderr,
	      "Usage: %s [-f] [-w] [-s 1280x720]\n"
	      "    -f: fullscreen\n"
	      "    -w: windowed\n"
	      "    -s: screen resolution\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  }


  init_sdl();
  on_exit(cleanup, 0);
  load();
  resize(scrWidth, scrHeight);
  SDL_ShowCursor(SDL_DISABLE);

  SDL_Event event;
  /*
  uint32_t last_draw = 0;
  uint32_t drawn = 0;
  uint32_t start = SDL_GetTicks();
  */
  bool running = true;

  while (running /*&& !global_done_flag*/) {
    while (SDL_PollEvent(&event)) {
      switch( event.type ) {
      case SDL_KEYDOWN:
	if (event.key.keysym.sym == SDLK_ESCAPE)
	  running = false;
	break;
      case SDL_QUIT:
        running = false;
        break;

      default:
        break;
      }
    }
    
    //uint32_t time = SDL_GetTicks();
    simpleRender();
    SDL_GL_SwapBuffers();
    //last_draw = time;
    //drawn++;
    //if (time > 80 * 1000) running = false;
  }
  /*
  uint32_t end = SDL_GetTicks();
  double runtime = (end - start) / 1000.0;
  fprintf(stderr, "Drawn %u frames\n", drawn);
  fprintf(stderr, "Runtime %.2f s, %.2f fps drawn\n",
	  runtime, drawn / runtime);
  */
  return 0;
}
