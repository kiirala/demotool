#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL.h>
#include <SDL_sound.h>
#include <time.h>

#include <GL/gl.h>

extern "C" void load();
extern "C" void unload();
extern "C" void render(double time);
extern "C" void resize(int width, int height);

//#define FULLSCREEN
static const int scrWidth = 1280;
static const int scrHeight = 720;
static const int scrBPP = 32;
//static const char *font_file = "/usr/share/fonts/ttf-bitstream-vera/Vera.ttf";

/* global decoding state. */
typedef struct
{
    Sound_Sample *sample;
    SDL_AudioSpec devformat;
    Uint8 *decoded_ptr;
    Uint32 decoded_bytes;
} PlaysoundAudioCallbackData;

/*
 * This variable is flipped to non-zero when the audio callback has
 *  finished playing the whole file.
 */
static volatile int global_done_flag = 0;

/*
 * The audio callback. SDL calls this frequently to feed the audio device.
 *  We decode the audio file being played in here in small chunks and feed
 *  the device as necessary. Other solutions may want to predecode more
 *  (or all) of the file, since this needs to run fast and frequently,
 *  but since we're only sitting here and waiting for the file to play,
 *  the only real requirement is that we can decode a given audio file
 *  faster than realtime, which isn't really a problem with any modern format
 *  on even pretty old hardware at this point.
 */
static void audio_callback(void *userdata, Uint8 *stream, int len)
{
  PlaysoundAudioCallbackData *data = (PlaysoundAudioCallbackData *) userdata;
  Sound_Sample *sample = data->sample;
  int bw = 0; /* bytes written to stream this time through the callback */

  while (bw < len)
    {
      unsigned int cpysize;  /* bytes to copy on this iteration of the loop. */

      if (data->decoded_bytes == 0) /* need more data! */
        {
	  /* if there wasn't previously an error or EOF, read more. */
	  if ( ((sample->flags & SOUND_SAMPLEFLAG_ERROR) == 0) &&
	       ((sample->flags & SOUND_SAMPLEFLAG_EOF) == 0) )
            {
	      data->decoded_bytes = Sound_Decode(sample);
	      data->decoded_ptr = (Uint8*)sample->buffer;
            } /* if */

	  if (data->decoded_bytes == 0)
            {
	      /* ...there isn't any more data to read! */
	      memset(stream + bw, '\0', len - bw);  /* write silence. */
	      global_done_flag = 1;
	      return;  /* we're done playback, one way or another. */
            } /* if */
        } /* if */

      /* we have data decoded and ready to write to the device... */
      cpysize = len - bw;  /* len - bw == amount device still wants. */
      if (cpysize > data->decoded_bytes)
	cpysize = data->decoded_bytes;  /* clamp to what we have left. */

      /* if it's 0, next iteration will decode more or decide we're done. */
      if (cpysize > 0)
        {
	  /* write this iteration's data to the device. */
	  memcpy(stream + bw, (Uint8 *) data->decoded_ptr, cpysize);

	  /* update state for next iteration or callback */
	  bw += cpysize;
	  data->decoded_ptr += cpysize;
	  data->decoded_bytes -= cpysize;
        } /* if */
    } /* while */
} /* audio_callback */

void init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
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
#ifdef FULLSCREEN
  videoFlags |= SDL_FULLSCREEN;
#endif

  SDL_Surface *surface = SDL_SetVideoMode(scrWidth, scrHeight, scrBPP,
					  videoFlags);
  if (!surface) {
    fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  SDL_WM_SetCaption("Go Space", "Go Space");
  /*
  SDL_AudioSpec aspec;
  aspec.freq = 44100;
  aspec.format = AUDIO_S16;
  aspec.channels = 2;
  aspec.samples = 2048;
  aspec.callback = audio_callback;
  aspec.userdata = 0;
  if (SDL_OpenAudio(&aspec, NULL) != 0) {
    fprintf(stderr, "Opening audio failed: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }
  */
}

PlaysoundAudioCallbackData data;
void load_song() {
  if (!Sound_Init()) { /* this calls SDL_Init(SDL_INIT_AUDIO) ... */
    fprintf(stderr, "Sound_Init() failed: %s.\n", Sound_GetError());
    SDL_Quit();
    exit(42);
  } /* if */

  memset(&data, '\0', sizeof (PlaysoundAudioCallbackData));
  data.sample = Sound_NewSampleFromFile("musa.xm", NULL, 65536);
  if (data.sample == NULL) {
    fprintf(stderr, "Couldn't load music: %s.\n", Sound_GetError());
    exit(4);
  }

  /*
   * Open device in format of the the sound to be played.
   *  We open and close the device for each sound file, so that SDL
   *  handles the data conversion to hardware format; this is the
   *  easy way out, but isn't practical for most apps. Usually you'll
   *  want to pick one format for all the data or one format for the
   *  audio device and convert the data when needed. This is a more
   *  complex issue than I can describe in a source code comment, though.
   */
  data.devformat.freq = data.sample->actual.rate;
  data.devformat.format = data.sample->actual.format;
  data.devformat.channels = data.sample->actual.channels;
  data.devformat.samples = 4096;  /* I just picked a largish number here. */
  data.devformat.callback = audio_callback;
  data.devformat.userdata = &data;
  if (SDL_OpenAudio(&data.devformat, NULL) < 0) {
    fprintf(stderr, "Couldn't open audio device: %s.\n", SDL_GetError());
    Sound_FreeSample(data.sample);
    exit(3);
  }

  SDL_PauseAudio(0);  /* SDL audio device is "paused" right after opening. */

  global_done_flag = 0;  /* the audio callback will flip this flag. */
}

void cleanup(int state, void * /*foo*/) {
  Sound_FreeSample(data.sample);  /* clean up SDL_Sound resources... */
  unload();
  SDL_Quit();
}

int main(int argc, char ** argv) {
  init_sdl();
  on_exit(cleanup, 0);
  load();
  resize(scrWidth, scrHeight);
  load_song();

  SDL_Event event;
  uint32_t last_draw = 0;
  uint32_t drawn = 0;
  uint32_t start = SDL_GetTicks();
  bool running = true;

  while (running && !global_done_flag) {
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
    
    uint32_t time = SDL_GetTicks();
    render(time / 1000.0);
    SDL_GL_SwapBuffers();
    last_draw = time;
    drawn++;
    if (time > 64 * 1000) running = false;
  }

  uint32_t end = SDL_GetTicks();
  double runtime = (end - start) / 1000.0;
  fprintf(stderr, "Drawn %u frames\n", drawn);
  fprintf(stderr, "Runtime %.2f s, %.2f fps drawn\n",
	  runtime, drawn / runtime);

  return 0;
}
