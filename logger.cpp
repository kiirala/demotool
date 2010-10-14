#include <cstdio>
#include <cstdarg>

#include "logger.h"

char msglevel[10][8] = {"unkn", "debug", "lvl2", "lvl3", "lvl4",
			"warn", "lvl6", "lvl7", "lvl8", "error"};

void Logger::message(char const *fname, int const line, int const level,
		     char const *message, ...) {
  va_list args;
  fprintf(stderr, "%s at %s:%d: ", msglevel[level], fname, line);
  va_start(args, message);
  vfprintf(stderr, message, args);
  va_end(args);
  fprintf(stderr, "\n");
}
