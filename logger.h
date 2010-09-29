#ifndef TT_LOGGER_H
#define TT_LOGGER_H

#define debug(str) Logger::message(__FILE__, __LINE__, 1, (str))
#define warn(str) Logger::message(__FILE__, __LINE__, 5, (str))
#define error(str) Logger::message(__FILE__, __LINE__, 9, (str))

class Logger {
public:
  static void message(char const *fname, int const line, int const level,
		      char const *message);
private:

};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TT_LOGGER_H */
