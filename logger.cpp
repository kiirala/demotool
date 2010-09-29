#include <stdio.h>

#include "logger.h"

char msglevel[10][8] = {"unkn", "debug", "lvl2", "lvl3", "lvl4",
			"warn", "lvl6", "lvl7", "lvl8", "error"};

void Logger::message(char const *fname, int const line, int const level,
		     char const *message) {
  fprintf(stderr, "%s: %s (at %s:%d)\n", msglevel[level], message,
	  fname, line);
}

