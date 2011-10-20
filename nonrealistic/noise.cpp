#include <math.h>
#include <stdlib.h>

#include "noise.h"

const int noise_size = 64;

// a: first value, b: second value, d: distance [0, 1]
static double interpolate(double a, double b, double d) {
  return a + (3 * d * d - 2 * d * d * d) * (b - a);
}

void perlin(double *values, int width, int height) {
  srand(1234);

  double vectors[noise_size][noise_size][2];

  for (int y = 0 ; y < noise_size ; ++y) {
    for (int x = 0 ; x < noise_size ; ++x) {
      double angle = (double)rand() / RAND_MAX * 2.0 * M_PI;
      vectors[y][x][0] = cos(angle);
      vectors[y][x][1] = sin(angle);
    }
  }

  for (int y = 0 ; y < height ; ++y) {
    for (int x = 0 ; x < width ; ++x) {
      double position_x = (double)x / width * (noise_size - 1);
      double position_y = (double)y / height * (noise_size - 1);
      int vector_x = (int)floor(position_x);
      int vector_y = (int)floor(position_y);

      double dx1 = position_x - vector_x;
      double dy1 = position_y - vector_y;
      double dx2 = position_x - (vector_x + 1);
      double dy2 = position_y - (vector_y + 1);

      double c1 = vectors[vector_y][vector_x][0] * dx1 +
	vectors[vector_y][vector_x][1] * dy1;
      double c2 = vectors[vector_y][vector_x + 1][0] * dx2 +
	vectors[vector_y][vector_x + 1][1] * dy1;
      double c3 = vectors[vector_y + 1][vector_x][0] * dx1 +
	vectors[vector_y + 1][vector_x][1] * dy2;
      double c4 = vectors[vector_y + 1][vector_x + 1][0] * dx2 +
	vectors[vector_y + 1][vector_x + 1][1] * dy2;

      double val_x1 = interpolate(c1, c2, dx1);
      double val_x2 = interpolate(c3, c4, dx1);
      values[y * width + x] = interpolate(val_x1, val_x2, dy1);
    }
  }
}
