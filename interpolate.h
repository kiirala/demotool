#ifndef DEMOTOOL_INTERPOLATE_H
#define DEMOTOOL_INTERPOLATE_H

struct Keyframe {
  double time;
  double value;
};

class Interpolate {
public:
  Interpolate(Keyframe const *keyframes, double start_time)
    :  start_time(start_time), keyframes(keyframes)
  { }

  double value(double time) const {
    double seek_time = time - start_time;
    Keyframe const *cur = keyframes;
    while ((cur + 1)->time >= 0.0 && (cur + 1)->time < seek_time)
      ++cur;
    Keyframe const *next = cur + 1;
    if (next->time < 0.0)
      return cur->value;
    double td = next->time - cur->time;
    double vd = next->value - cur->value;
    double t = seek_time - cur->time;
    return cur->value + (t / td) * vd;
  }

private:
  double start_time;
  Keyframe const *keyframes;
};


/*
Local Variables:
mode:c++
End:
*/
#endif /* DEMOTOOL_INTERPOLATE_H */
