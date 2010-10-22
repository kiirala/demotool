#ifndef DEMOTOOL_INTERPOLATE_H
#define DEMOTOOL_INTERPOLATE_H

struct Keyframe1 {
  double time;
  double value;
};

template <int size>
struct Keyframe {
  double time;
  double value[size];
};

class Interpolate1 {
public:
  Interpolate1(Keyframe1 const *keyframes, double start_time)
    :  start_time(start_time), loop(-1), keyframes(keyframes)
  { }

  double value(double time) const {
    double seek_time = time - start_time;
    if (loop > 0.0)
      seek_time = fmod(seek_time, loop);

    Keyframe1 const *cur = keyframes;
    while ((cur + 1)->time >= 0.0 && (cur + 1)->time < seek_time)
      ++cur;
    Keyframe1 const *next = cur + 1;
    double td = next->time - cur->time;
    if (next->time < 0.0) {
      if (loop <= 0.0)
	return cur->value;
      next = keyframes;
      td = loop - cur->time;
    }
    double vd = next->value - cur->value;
    double t = seek_time - cur->time;
    return cur->value + (t / td) * vd;
  }

  void set_start(double start) {
    start_time = start;
  }

  void set_loop(double loop) {
    this->loop = loop;
  }

private:
  double start_time, loop;
  Keyframe1 const *keyframes;
};

template <int size>
class Interpolate {
public:
  Interpolate(Keyframe<size> const *keyframes, double start_time)
    :  start_time(start_time), loop(-1), smoothness(0), keyframes(keyframes)
  { }

  float* value(double time) {
    double seek_time = time - start_time;
    if (loop > 0)
      seek_time = fmod(seek_time, loop);

    Keyframe<size> const *cur = keyframes;
    while ((cur + 1)->time >= 0.0 && (cur + 1)->time < seek_time)
      ++cur;
    Keyframe<size> const *next = cur + 1;
    double td = next->time - cur->time;
    if (next->time < 0.0) {
      if (loop <= 0.0) {
	for (int i = 0 ; i < size ; ++i) {
	  output[i] = cur->value[i];
	}
	return output;
      }
      next = keyframes;
      td = loop - cur->time;
    }
    double t = seek_time - cur->time;
    double step = smoothness * sin((t / td) * (M_PI / 2.0)) +
      (1.0 - smoothness) * (t / td);
    for (int i = 0 ; i < size ; ++i) {
      double vd = next->value[i] - cur->value[i];
      output[i] = cur->value[i] + step * vd;
    }
    return output;
  }

  void set_start(double start) {
    start_time = start;
  }

  void set_loop(double loop) {
    this->loop = loop;
  }

  void set_smoothness(double smoothness) {
    this->smoothness = smoothness;
  }

private:
  double start_time, loop, smoothness;
  Keyframe<size> const *keyframes;
  float output[size];
};

/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_INTERPOLATE_H */
