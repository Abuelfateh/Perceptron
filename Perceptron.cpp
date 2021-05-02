#include "Perceptron.h"


int signum(double ws) {
  return 0.0 == ws ? 0 : (ws > 0 ? 1 : -1);
}

int hard_limit(double ws) {
  return ws > 0 ? 1 : 0;
}


Perceptron::Perceptron() {
  m_activation = signum;
  m_weight = nullptr;
  m_eta = 1.0;
  html = nullptr;
  m_report = true;
  html_file = new char[12];
  strcpy_s(html_file, 12, "report.html");
  //strcpy(html_file, "report.html");
  html_file[11] = '\0';
}

Perceptron::~Perceptron() {
  delete[] html_file;
  if (nullptr != m_weight)
    delete m_weight;
}

void Perceptron::enableReport(bool status) {
  m_report = status;
}

void Perceptron::init(const Vector &weight, double eta, int (*activation)(double)) {
  if (nullptr != m_weight)
    delete m_weight;

  m_eta = eta;
  m_weight = new Vector(weight);
  m_activation = activation;
  if (m_report) {
    if (0 == fopen_s(&html, html_file, "w")) {
      fprintf(html, "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" /><title>ANN - Perceptron</title><style>table{width:100%%;color:#000;border-spacing:0;border-collapse:collapse;}thead,tbody{vertical-align:middle;}td,th{vertical-align:middle;padding:.75rem;text-align:center;border:1px solid #333;}</style></head><body>");
      fclose(html);
    }
    else {
      m_report = false;
      html = nullptr;
    }
  }
}

void Perceptron::init(const Vector &weight) {
  init(weight, 1.0, signum);
}

void Perceptron::init(const Vector &weight, double eta) {
  init(weight, eta, signum);
}

void Perceptron::init(const Vector &weight, int (*activation)(double)) {
  init(weight, 1.0, activation);
}

int Perceptron::exec(Vector &input) {
  if (m_weight && input.length() == m_weight->length()) {
    return m_activation(*m_weight * input);
  }
  return 0;
}

bool Perceptron::epoch(Pattern *pts, int plen, int iteration) {
  bool update, htmlOpened = false, ret = false;
  int y, j, i, wlen = m_weight->length();
  double ws, wi;

  if (m_report && 0 == fopen_s(&html, html_file, "a"))
    htmlOpened = true;

  if (htmlOpened) {
    fprintf(html, "<h3><u>Iteration %d</u></h3>", iteration);
    fprintf(html, "<table><thead><tr><th>Pattern Applied</th><th>Weight Applied</th><th>W<sup>T</sup>X</th><th>y=%s<br />(W<sup>T</sup>X)</th><th>d</th><th>Update</th><th>New Weight</th></tr></thead><tbody>", m_activation == signum ? "sgn" : "H.L.");
  }

  for (i = 0; i < plen; i++) {
    ws = *m_weight * pts[i].input;
    y = m_activation(ws);
    update = y != pts[i].d;

    if (htmlOpened) {
      fprintf(html, "<tr><td>[");
      for (j = 0; j < wlen; j++) {
        if (j > 0)
          fprintf(html, ", %.2f", pts[i].input.get(j));
        else
          fprintf(html, "%.2f", pts[i].input.get(j));
      }
      fprintf(html, "]</td><td>[");
      for (j = 0; j < wlen; j++) {
        if (j > 0)
          fprintf(html, ", %.2f", m_weight->get(j));
        else
          fprintf(html, "%.2f", m_weight->get(j));
      }
      fprintf(html, "]</td><td>%+.2f</td><td>%+d</td><td>%+d</td><td>%s</td>", ws, y, pts[i].d, update ? "Yes" : "No");
    }

    if (update) {
      ret = true;
      for (j = 0; j < wlen; j++) {
        wi = m_weight->get(j);
        m_weight->set(j, wi + (m_eta * (pts[i].d - y) * pts[i].input.get(j)));
      }
    }

    if (htmlOpened) {
      fprintf(html, "<td>[");
      for (j = 0; j < wlen; j++) {
        if (j > 0)
          fprintf(html, ", %.2f", m_weight->get(j));
        else
          fprintf(html, "%.2f", m_weight->get(j));
      }
      fprintf(html, "]</td></tr>");
    }
  }

  if (htmlOpened) {
    fprintf(html, "</tbody></table><br />");
    fclose(html);
  }

  return ret;
}

int Perceptron::learn(Pattern *pts, int plen) {
  int i = 1;
  while (i < MAX_ITERATION_COUNT && epoch(pts, plen, i))
    i++;

  return i;
}

void Perceptron::report() {
  if (m_report && 0 == fopen_s(&html, html_file, "a")) {
    fprintf(html, "</body></html>");
    fclose(html);
    ShellExecuteA(NULL, "open", html_file, NULL, NULL, SW_SHOWNORMAL);
  }
}

void Perceptron::_swap(Pattern *pts, int dest, int src) {
  Pattern p;

  p.d = pts[dest].d;
  p.input.copy(pts[dest].input);

  pts[dest].d = pts[src].d;
  pts[dest].input.copy(pts[src].input);

  pts[src].d = p.d;
  pts[src].input.copy(p.input);
}

void Perceptron::permutate(Pattern *pts, int start, int end, void(*callback)(Pattern *, void *), void *args) {
  if (start == end) {
    callback(pts, args);
    return;
  }

  for (int i = start; i <= end; i++) {
    if (i != start)
      _swap(pts, start, i);

    permutate(pts, start + 1, end, callback, args);

    // Backtrack
    if (i != start)
      _swap(pts, start, i);
  }
}

void Perceptron::debug() {
  int i = 0, len = m_weight->length();
  printf("\n---------- DEBUG ----------\n");
  printf("W: [");
  for (; i < len; i++) {
    if (i > 0)
      printf(", ");

    printf("%lf", m_weight->get(i));
  }
  printf("]\neta: %lf\nactv: %s\n", m_eta, m_activation == hard_limit ? "H.L." : "signum");
  printf("----------  END  ----------\n");
}

