#ifndef PERCEPTRON_H
#define PERCEPTRON_H 1

#include "Abuelfateh.h"
#include "Vector.h"

#define MAX_ITERATION_COUNT 1000

int signum(double);
int hard_limit(double);

struct Pattern {
  Vector input;
  int d;
};

class Perceptron {

  int (*m_activation)(double);
  Vector *m_weight;
  double m_eta;
  FILE *html;
  bool m_report;
  char *html_file;

  void _swap(Pattern *, int, int);

  public:
  Perceptron();
  ~Perceptron();

  void init(const Vector &);
  void init(const Vector &, double);
  void init(const Vector &, int (*)(double));
  void init(const Vector &, double, int (*)(double));

  void enableReport(bool);

  int exec(Vector &);

  bool epoch(Pattern *, int, int = 1);
  int learn(Pattern *, int);

  void permutate(Pattern *, int start, int end, void(*callback)(Pattern *, void *), void *args);

  void report();

  void debug();

};

#endif // !PERCEPTRON_H
