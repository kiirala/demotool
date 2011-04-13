#ifndef TT_LOGGER_H
#define TT_LOGGER_H

#include <GL/gl.h>

#define debug(...) Logger::message(__FILE__, __LINE__, 1, __VA_ARGS__)
#define warn(...) Logger::message(__FILE__, __LINE__, 5, __VA_ARGS__)
#define error(...) Logger::message(__FILE__, __LINE__, 9, __VA_ARGS__)
#define logGL() {GLenum err = glGetError(); if (err != GL_NO_ERROR) Logger::glerror(__FILE__, __LINE__, err);}

class Logger {
public:
  static void message(char const *fname, int const line, int const level,
		      char const *message, ...)
    __attribute__((format(printf, 4, 5)));
  static void glerror(char const *fnamme, int const line, GLenum const error);

private:

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TT_LOGGER_H */
