#ifndef VECTOR_H
#define VECTOR_H 1

#include "Abuelfateh.h"

class Vector {

  double *m_vector;
  int m_length;

  public:
  Vector();
  Vector(double *, int);
  Vector(const Vector &);
  ~Vector();

  void copy(const Vector &);

  void init(double *, int);
  void clean();

  double get(int);
  void set(int, const double);

  int length();

  double operator *(const Vector &);

  void debug();

};

#endif // !VECTOR_H
