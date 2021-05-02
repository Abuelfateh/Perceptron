#include "FileParser.h"

#define MAX_BUF_SIZE 1024


FileParser::FileParser() {
  m_patterns = nullptr;
  m_patternCount = 0;
  m_eta = 1;
  m_flag = 0;
  m_errno = 0;
  m_lineno = 0;
  m_expected = 0;
}

void FileParser::parse(const char *path) {
  FILE *fp = NULL;
  char *buffer = NULL;
  long int bLen, rLen;

  if (0 == fopen_s(&fp, path, "r")) {
    fseek(fp, 0, SEEK_END);
    bLen = ftell(fp);
    rewind(fp);
    bLen++;
    buffer = (char *)malloc(bLen * sizeof(char));
    if (buffer) {
      memset(buffer, '\0', bLen * sizeof(char));
      rLen = fread(buffer, sizeof(char), bLen, fp);
      _parse(buffer);

      free(buffer);
    }
    else {
      m_errno = 2;
    }

    fclose(fp);
  }
  else {
    m_errno = 1;
  }
}

FileParser::~FileParser() {
  if (nullptr != m_patterns) {
    delete[] m_patterns;
  }
}

void FileParser::consume(char *buffer, long int &i) {
  while (' ' == buffer[i] || '\t' == buffer[i] || '\r' == buffer[i] || '\n' == buffer[i]) {
    if ('\n' == buffer[i])
      m_lineno++;

    i++;
  }
}

void FileParser::consume(char *buffer, long int &i, char ch) {
  while (ch == buffer[i] || ' ' == buffer[i] || '\t' == buffer[i] || '\r' == buffer[i] || '\n' == buffer[i]) {
    if ('\n' == buffer[i])
      m_lineno++;

    i++;
  }
}

int _countPatterns(const char *text) {
  long int i = 0;
  int count = 0;

  for (; text[i]; i++) {
    if ('#' == text[i]) {
      while (text[i] && '\n' != text[i])
        i++;

      continue;
    }
    if ('p' == (text[i]) && text[i + 1] && ':' == text[i + 1]) {
      i++;
      count++;
    }
  }

  return count;
}

bool _parsePattern(char *text, long int &i, char *buffer, Pattern *pattern, int d) {
  double *arr = nullptr;
  int j = 0, l;
  buffer[j++] = '1';
  buffer[j++] = ',';
  while (text[i] && j < MAX_BUF_SIZE && ']' != text[i])
    buffer[j++] = text[i++];

  if (']' != text[i])
    return false;

  buffer[j] = '\0';
  arr = toDoubleArray(buffer, l);
  if (arr && l > 0) {
    pattern->d = d;
    pattern->input.init(arr, l);

    delete[] arr;
    return true;
  }

  return false;
}

void FileParser::_parse(char *text) {
  long int i, j;
  int l, p = 0;
  char buffer[MAX_BUF_SIZE + 1];
  double *arr = nullptr;
  m_patternCount = _countPatterns(text);

  if (m_patternCount > 0)
    m_patterns = new Pattern[m_patternCount];

  if (m_patterns) {
    m_lineno = 1;
    for (i = 0; text[i]; i++) {
      consume(text, i);

      if ('#' == text[i]) {
        while (text[i] && '\n' != text[i])
          i++;

        m_lineno++;
      }
      else if ('P' == (text[i] & 223) && text[i + 1] && ':' == text[i + 1]) {
        i += 2;

        if ('+' == text[i] || '1' == text[i]) {
          i++;
          if ('1' == text[i]) i++;
          consume(text, i, '=');
          if ('[' == text[i]) {
            consume(text, i, '[');
            if (!_parsePattern(text, i, buffer, &m_patterns[p++], 1)) {
              m_errno = 5;
              m_expected = '+';
              break;
            }
          }
          else {
            m_errno = 4;
            m_expected = '[';
            break;
          }
        }
        else if ('-' == text[i]) {
          i++;
          if ('1' == text[i]) i++;
          consume(text, i, '=');
          if ('[' == text[i]) {
            consume(text, i, '[');
            if (!_parsePattern(text, i, buffer, &m_patterns[p++], -1)) {
              m_errno = 5;
              m_expected = '-';
              break;
            }
          }
          else {
            m_errno = 4;
            m_expected = '[';
            break;
          }
        }
        else {
          m_errno = 5;
          m_expected = 9;
          break;
        }
      }
      else {
        j = 0;
        while (text[i] && EOF != text[i] && j < MAX_BUF_SIZE && ' ' != text[i] && '\t' != text[i] && '\r' != text[i] && '\n' != text[i] && '=' != text[i])
          buffer[j++] = text[i++];

        buffer[j] = '\0';
        consume(text, i, '=');

        if (0 == _stricmp(buffer, "eta")) {
          j = 0;
          if ('-' == text[i])
            buffer[j++] = text[i++];

          while (text[i] && j < MAX_BUF_SIZE && ('.' == text[i] || (text[i] >= '0' && text[i] <= '9')))
            buffer[j++] = text[i++];

          buffer[j] = '\0';
          m_eta = toDouble(buffer);
        }
        else if (0 == _stricmp(buffer, "activation")) {
          j = 0;
          while (text[i] && j < MAX_BUF_SIZE && ' ' != text[i] && '\t' != text[i] && '\n' != text[i])
            buffer[j++] = text[i++];

          buffer[j] = '\0';
          if (0 == _stricmp(buffer, "signum"))
            m_flag |= FP_ACTV_SIGNUM;
          else if (0 == _stricmp(buffer, "hard_limit"))
            m_flag |= FP_ACTV_HL;
          else
            m_flag = 0;
        }
        else if (0 == _stricmp(buffer, "weight")) {
          if ('[' == text[i]) {
            consume(text, i, '[');
            j = 0;
            while (text[i] && j < MAX_BUF_SIZE && ']' != text[i])
              buffer[j++] = text[i++];

            buffer[j] = '\0';
            if (']' == text[i]) {
              arr = toDoubleArray(buffer, l);
              if (arr && l > 0) {
                m_weight.init(arr, l);
                delete[] arr;
              }
              else {
                m_errno = 2;
                break;
              }
            }
            else {
              m_errno = 4;
              m_expected = ']';
              break;
            }
          }
          else {
            m_errno = 4;
            m_expected = '[';
            break;
          }
        }
        else if (strlen(buffer) > 0) {
          m_errno = 3;
          break;
        }
      }

    }//EOF
  }
  else {
    m_errno = 2;
  }
}

int FileParser::error() {
  return m_errno;
}

double FileParser::eta() {
  return m_eta;
}

bool FileParser::actvSignum() {
  return m_flag & FP_ACTV_SIGNUM;
}

bool FileParser::actvHL() {
  return m_flag & FP_ACTV_HL;
}

int FileParser::patternCount() {
  return m_patternCount;
}

Vector *FileParser::weight() {
  return &m_weight;
}

Pattern *FileParser::pattern() {
  return m_patterns;
}

void FileParser::printErrmsg() {
  switch (m_errno) {
    case 1:
      printf("File not found.\nmake sure to put the text file perceptron.txt in the same location to exe file\nor call the program with file path\nPerceptron.exe \"/path/to/file\"\n");
      break;
    case 2:
      printf("No enough memory, to load the file.\n");
      break;
    case 3:
      printf("Invalid parameter in line %d.\n", m_lineno);
      break;
    case 4:
      printf("Expected %c on line %d.\n", m_expected, m_lineno);
      break;
    case 5:
      if (9 == m_expected)
        printf("Invalid desired value for pattern, in line %d.\n", m_lineno);
      else
        printf("Invalid pattern in %c class, in line %d.\n", m_expected, m_lineno);
      break;
    default:
      printf("No Error Found.\n");
  }
}