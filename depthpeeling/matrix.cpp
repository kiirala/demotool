#include "matrix.h"

class Matrix2 {
public:
  double val[4];

  Matrix2() { }
  Matrix2(double const a, double const b, double const c, double const d) {
    val[0] = a;
    val[1] = b;
    val[2] = c;
    val[3] = d;
  }

  const Matrix2 inverse() const {
    Matrix2 res;

    double factor = 1.0 / (val[0] * val[3] - val[1] * val[2]);
    res.val[0] = val[3] * factor;
    res.val[3] = val[0] * factor;
    res.val[1] = -val[1] * factor;
    res.val[2] = -val[2] * factor;

    return res;
  }

  const Matrix2 operator* (Matrix2 const &other) const {
    Matrix2 res;

    res.val[0] = val[0] * other.val[0] + val[2] * other.val[1];
    res.val[1] = val[1] * other.val[0] + val[3] * other.val[1];
    res.val[2] = val[0] * other.val[2] + val[2] * other.val[3];
    res.val[3] = val[1] * other.val[2] + val[3] * other.val[3];
    
    return res;
  }

  const Matrix2 operator+ (Matrix2 const &other) const {
    Matrix2 res;
    for (int i = 0 ; i < 4 ; i++) {
      res.val[i] = val[i] + other.val[i];
    }
    return res;
  }

  const Matrix2 operator- (Matrix2 const &other) const {
    Matrix2 res;
    for (int i = 0 ; i < 4 ; i++) {
      res.val[i] = val[i] - other.val[i];
    }
    return res;
  }

  const Matrix2 operator- () const {
    Matrix2 res;
    for (int i = 0 ; i < 4 ; i++) {
      res.val[i] = -val[i];
    }
    return res;
  }
};

const Matrix Matrix::inverse() const {
  Matrix2 a(val[0], val[1], val[4], val[5]);
  Matrix2 b(val[8], val[9], val[12], val[13]);
  Matrix2 c(val[2], val[3], val[6], val[7]);
  Matrix2 d(val[10], val[11], val[14], val[15]);

  Matrix2 a_inv = a.inverse();
  Matrix2 schur = (d - c * a_inv * b).inverse();

  Matrix2 res_a = a_inv + a_inv * b * schur * c * a_inv;
  Matrix2 res_b = -a_inv * b * schur;
  Matrix2 res_c = -schur * c * a_inv;
  Matrix2 res_d = schur;

  Matrix ret;
  ret.val[0] = res_a.val[0];
  ret.val[1] = res_a.val[1];
  ret.val[2] = res_c.val[0];
  ret.val[3] = res_c.val[1];
  ret.val[4] = res_a.val[2];
  ret.val[5] = res_a.val[3];
  ret.val[6] = res_c.val[2];
  ret.val[7] = res_c.val[3];
  ret.val[8] = res_b.val[0];
  ret.val[9] = res_b.val[1];
  ret.val[10] = res_d.val[0];
  ret.val[11] = res_d.val[1];
  ret.val[12] = res_b.val[2];
  ret.val[13] = res_b.val[3];
  ret.val[14] = res_d.val[2];
  ret.val[15] = res_d.val[3];
  return ret;
}

const Matrix Matrix::transpose() const {
  Matrix ret;

  for (int i = 0 ; i < 4 ; i++) {
    for (int j = 0 ; j < 4 ; j++) {
      ret.val[i + 4 * j] = val[j + 4 * i];
    }
  }

  return ret;
}

const Matrix Matrix::transform(double const x, double const y,
			       double const z) const
{
  Matrix trans;
  trans.val[12] = x;
  trans.val[13] = y;
  trans.val[14] = z;

  //trans *= (*this);
  return Matrix(*this) * trans;
}

const Matrix Matrix::rotate(double const angle, double const x,
			    double const y, double const z) const
{
  Matrix trans;

  Point u(x, y, z);
  u.normalize();

  for (int i = 0 ; i < 3 ; i++) {
    double u_val;
    if (i == 0) u_val = u.x;
    else if (i == 1) u_val = u.y;
    else u_val = u.z;

    for (int j = 0 ; j < 3 ; j++) {
      double ut_val, s_val, i_val;

      if (j == 0) ut_val = u.x;
      else if (j == 1) ut_val = u.y;
      else ut_val = u.z;

      if (i == 0 && j == 1) s_val = -u.z;
      else if (i == 0 && j == 2) s_val = u.y;
      else if (i == 1 && j == 0) s_val = u.z;
      else if (i == 1 && j == 2) s_val = -u.x;
      else if (i == 2 && j == 0) s_val = -u.y;
      else if (i == 2 && j == 1) s_val = u.x;
      else s_val = 0.0;

      if (i == j) i_val = 1.0;
      else i_val = 0.0;

      trans.val[i + j * 4] = u_val * ut_val
	+ cos(angle) * (i_val - u_val * ut_val)
	+ sin(angle) * s_val;
    }
  }

  //trans *= (*this);
  return Matrix(*this) * trans;
}

const Matrix Matrix::scale(double const x, double const y,
			   double const z) const
{
  Matrix trans;

  trans.val[0] = x;
  trans.val[5] = y;
  trans.val[10] = z;

  //trans *= (*this);
  return Matrix(*this) * trans;
}
