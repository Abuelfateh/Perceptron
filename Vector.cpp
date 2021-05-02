#include "Vector.h"

Vector::Vector() {
  m_vector = nullptr;
  m_length = 0;
}

Vector::Vector(double *vector, int length) {
  init(vector, length);
}

Vector::Vector(const Vector &V) {
  init(V.m_vector, V.m_length);
}

void Vector::clean() {
  if (m_length > 0 && m_vector != nullptr) {
    delete[] m_vector;
    m_length = 0;
  }
}

void Vector::init(double *vector, int length) {
  if (length > 0) {
    clean();
    m_vector = new double[length];
    if (m_vector) {
      m_length = length;
      for (int i = 0; i < length; i++)
        m_vector[i] = vector[i];
    }
  }
}

void Vector::copy(const Vector &V) {
  init(V.m_vector, V.m_length);
}

Vector::~Vector() {
  clean();
}

double Vector::get(int index) {
  if (index < m_length && index >= 0)
    return m_vector[index];

  return 0;
}

void Vector::set(int index, const double data) {
  if (index < m_length && index >= 0)
    m_vector[index] = data;
}

int Vector::length() {
  return m_length;
}

double Vector::operator *(const Vector &rhs) {
  double ret = 0;
  if (m_length > 0 && m_length == rhs.m_length) {
    for (int i = 0; i < m_length; i++)
      ret += (m_vector[i] * rhs.m_vector[i]);
  }

  return ret;
}

void Vector::debug() {
  printf("\n---------- DEBUG (Vector) ----------\n");
  printf("[");
  for (int i = 0; i < m_length; i++) {
    if (i > 0)
      printf(", ");

    printf("%lf", m_vector[i]);
  }
  printf("]\n");
  printf("---------- -------------- ----------\n");
}