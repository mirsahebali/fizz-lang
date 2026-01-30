#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

String String_from(const char *str) {
  int32_t len = strlen(str);
  char *buf = malloc(len + 1);
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

String str_clone(String *value) {
  assert(value != NULL);
  char *buf = malloc(value->length + 1);
  int32_t len = value->length;
  assert(buf != NULL);
  snprintf(buf, len + 1, "%s", value->chars);
  return (String){
      .chars = buf,
      .length = len,
  };
}

String str_move(String *src) {
  assert(src != NULL || src->chars != NULL || src->length != 0);
  char *out = malloc(src->length + 1);
  int32_t len = src->length;
  strncpy(out, src->chars, len + 1);
  free_string(src);

  return (String){
      .chars = out,
      .length = len,
  };
}
bool is_letter(char ch) {
  return (ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A') || ch == '_';
}

bool is_digit(char ch) { return ch >= '0' && ch <= '9'; }

unsigned char count_digits(int input) {
  if (input < 0)
    input = (input == INT_MIN) ? INT_MAX : -input;
  if (input < 10)
    return 1;
  if (input < 100)
    return 2;
  if (input < 1000)
    return 3;
  if (input < 10000)
    return 4;
  if (input < 100000)
    return 5;
  if (input < 1000000)
    return 6;
  if (input < 10000000)
    return 7;
  if (input < 100000000)
    return 8;
  if (input < 1000000000)
    return 9;

  return 0;
}

void free_string(String *s) {
  if (!s)
    return;

  free(s->chars);
  s->chars = NULL;
  s->length = 0;
}

StringArray string_array_init(int32_t capacity) {
  int32_t cap = capacity <= 0 ? 16 : capacity;

  String *data = malloc(sizeof(String) * cap);

  return (StringArray){.data = data, .size = 0, .capacity = cap};
}
bool string_array_reserve(StringArray *self) {
  assert(self != NULL);
  if (self->size < self->capacity)
    return true;

  int32_t new_cap = self->capacity * 2;
  if (new_cap <= 0)
    return false;

  String *ret = realloc(self->data, sizeof(String *) * new_cap);
  if (!ret)
    return false;

  self->data = ret;
  self->capacity = new_cap;
  return true;
}
int32_t string_array_push(StringArray *self, String data) {
  String new_data = str_move(&data);
  assert(self != NULL);
  if (string_array_reserve(self)) {
    self->data[self->size++] = new_data;
  } else {
    printf("Cannot relloc memory into StringArray\n");
    assert(false);
  }
  return self->size;
}

int32_t string_array_size(const StringArray *self) { return self->size; }
int32_t string_array_capacity(const StringArray *self) {
  return self->capacity;
}
String string_array_get(const StringArray *self, int32_t index) {
  assert(index >= 0 && index < self->size);
  return self->data[index];
}

void print_string_array(const StringArray *arr) {
  for (int32_t i = 0; i < arr->size; i++) {
    String s = string_array_get(arr, i);
    printf("%s\n", s.chars);
  }
}

void free_string_array(StringArray *self) {
  for (int32_t i = 0; i < self->size; i++) {
    free_string(&self->data[i]);
  }

  free(self->data);
}
