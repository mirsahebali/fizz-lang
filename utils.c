#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

String String_from(const char *str) {
  int32_t len = strlen(str);
  char *buf = (char *)malloc(sizeof(char) * (len + 1));
  char *dest = strcpy(buf, str);
  return (String){dest, strlen(str)};
}

String String_new_n(int32_t len) {
  char *buf = (char *)malloc(sizeof(char) * len);

  return (String){buf, len};
}

String concat_char(String *input, char ch) {
  size_t new_len = input->length + 1;
  char *buf = (char *)malloc(sizeof(char) * (new_len + 1));
  assert(buf != NULL);

  char new_ch[2] = {ch, '\0'};

  snprintf(buf, new_len + 1, "%s%s", input->chars, new_ch);

  return (String){buf, new_len};
}
String concat_str(String *input, const char *str) {
  size_t new_len = input->length + strlen(str);
  char *buf = (char *)malloc(sizeof(char) * (new_len + 1)); // +1 for '\0'

  assert(buf != NULL);

  snprintf(buf, new_len + 1, "%s%s", input->chars, str);

  return (String){buf, new_len};
}

bool cmp_str(String *left, String *right) {
  return (strcmp(left->chars, right->chars) == 0) &&
         (left->length == right->length);
}

char char_at_str(String *input, int32_t index) {

  assert(index < input->length && index >= 0);

  return input->chars[index];
}

String str_from_char(char ch) {
  char *c = (char *)malloc(sizeof(char) * 2);
  c[0] = ch;
  c[1] = '\0';
  assert(c != NULL);

  return (String){c, 1};
}

String substr_range(String *input, int32_t begin, int32_t steps) {
  assert((begin >= 0) && (begin < input->length));
  assert(begin + steps < input->length);

  String out = String_new_n(steps + 1);
  for (int i = begin; i < begin + steps; i++) {
    out.chars[i - begin] = input->chars[i];
  }

  out.chars[steps] = '\0';

  out.length--;

  return out;
}

bool is_letter(char ch) {
  return (ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A') || ch == '_';
}

bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

void free_string(String *s) {
  if (!s)
    return;

  free(s->chars);
  s->chars = NULL;
  s->length = 0;
}
