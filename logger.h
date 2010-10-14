#ifndef TT_LOGGER_H
#define TT_LOGGER_H

#define debug(...) Logger::message(__FILE__, __LINE__, 1, __VA_ARGS__)
#define warn(...) Logger::message(__FILE__, __LINE__, 5, __VA_ARGS__)
#define error(...) Logger::message(__FILE__, __LINE__, 9, __VA_ARGS__)

class Logger {
public:
  static void message(char const *fname, int const line, int const level,
		      char const *message, ...)
    __attribute__((format(printf, 4, 5)));
private:

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TT_LOGGER_H */
