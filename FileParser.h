#ifndef FILE_PARSER_H
#define FILE_PARSER_H 1

#include "Abuelfateh.h"
#include "Perceptron.h"
#include "Vector.h"

#define FP_ACTV_SIGNUM   0x0001
#define FP_ACTV_HL       0x0002


class FileParser {
  private:
  Pattern *m_patterns;
  Vector m_weight;
  int m_patternCount, m_errno, m_lineno;
  double m_eta;
  unsigned char m_flag;
  char m_expected;

  void consume(char *, long int &);
  void consume(char *, long int &, char);

  void _parse(char *);

  public:
  FileParser();
  ~FileParser();

  void parse(const char *);
  int error();
  void printErrmsg();

  double eta();
  bool actvSignum();
  bool actvHL();
  int patternCount();
  Vector *weight();
  Pattern *pattern();

};

#endif // !FILE_PARSER_H
