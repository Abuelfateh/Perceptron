#include "Abuelfateh.h"
#include "Vector.h"
#include "Perceptron.h"
#include "FileParser.h"


int main(int argc, char *argv[]) {

  FileParser fp = FileParser();
  Perceptron P;
  Vector inputV = Vector();
  char c;
  int len;
  double *inp;


  fp.parse("perceptron.txt");

  if (0 != fp.error()) {
    fp.printErrmsg();
    goto End;
  }

  P.init(*fp.weight(), fp.eta(), fp.actvHL() ? hard_limit : signum);
  //P.debug();
  P.enableReport(true);
  P.learn(fp.pattern(), fp.patternCount());
  printf("Would you like to generate HTML report? (y) ");
  c = _getch();
  if ('Y' == (c & 223)) {
    printf("%c\n", c);
    P.report();
  }

  printf("\nWould you like to test the Perceptron? (y) ");
  c = _getch();
  if ('Y' == (c & 223)) {
    printf("%c\n", c);

    len = fp.weight()->length();
    TestResult:
    inp = new double[len];
    printf("Enter %d numeric values separated by Enter:\n", len - 1);
    inp[0] = 1.0;
    for (int i = 1; i < len; i++) {
      inp[i] = getDouble();
    }
    inputV.init(inp, len);
    delete[] inp;
    printf("Result: %d\n", P.exec(inputV));
    printf("\nWould you like to test another value? (y) ");
    c = _getch();
    if ('Y' == (c & 223)) {
      printf("%c\n", c);
      goto TestResult;
    }
  }

  End:
  printf("\nDeveloped by Ahmed Abuelfateh...");
  (void)_getch();
  return 0;
}