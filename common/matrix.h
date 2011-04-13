#ifndef TT_MATRIX_H
#define TT_MATRIX_H

#include "types.h"

class Matrix {
public:
  Matrix() {
    for (int i = 0 ; i < 16 ; i++) {
      val[i] = 0.0;
    }
    val[0] = val[5] = val[10] = val[15] = 1.0;
  }

  Matrix(Matrix const &other) {
    for (int i = 0 ; i < 16 ; i++) {
      this->val[i] = other.val[i];
    }
  }

  Matrix(double const val[16]) {
    for (int i = 0 ; i < 16 ; i++) {
      this->val[i] = val[i];
    }
  }

  Matrix & operator= (Matrix const &other) {
    for (int i = 0 ; i < 16 ; i++)
      val[i] = other.val[i];
    return *this;
  }

  Matrix & operator*= (Matrix const &other) {
    double res[16];
    
    for (int i = 0 ; i < 4 ; i++) {
      for (int j = 0 ; j < 4 ; j++) {
	double sum = 0;
	for (int k = 0 ; k < 4 ; k++) {
	  sum += val[i + k * 4] * other.val[k + j * 4];
	}
	res[i + j * 4] = sum;
      }
    }

    for (int i = 0 ; i < 16 ; i++)
      val[i] = res[i];

    return *this;
  }

  const Matrix operator* (Matrix const &other) const {
    Matrix res(*this);
    res *= other;
    return res;
  }

  const Point operator* (Point const &p) const {
    Point res;

    res.x = p.x * val[0] + p.y * val[4] + p.z * val[8] + val[12];
    res.y = p.x * val[1] + p.y * val[5] + p.z * val[9] + val[13];
    res.z = p.x * val[2] + p.y * val[6] + p.z * val[10] + val[14];

    return res;
  }

  const double* glMatrix() const {
    return val;
  }

  const Matrix inverse() const;
  const Matrix transpose() const;
  
  const Matrix transform(double const x, double const y, double const z) const;
  const Matrix rotate(double const angle, double const x, double const y,
		      double const z) const;
  const Matrix scale(double const x, double const y, double const z) const;

private:
  double val[16];
};

/*
Local Variables:
mode:c++
End:
*/
#endif /* TT_MATRIX_H */
