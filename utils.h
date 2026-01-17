#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  char *chars;
  int32_t length;
} String;

// creates a new string with a inferred length
String String_from(const char *str);

// initialize a new string with a set capacity
String String_new_n(int32_t len);

// creates a new string with a given length
String String_from_n(const char *str, int32_t len);

String str_from(const char *);
String str_clone(String *);

String concat_char(String *, char);
String concat_str(String *, const char *);

// Create a new single character string
String str_from_char(char ch);

String substr_range(String *, int32_t begin, int32_t end);

char char_at_str(String *, int32_t);

bool cmp_str(String *left, String *right);

bool is_letter(char);

bool is_digit(char);

// Always call this after reassigning strings or being done after use
void free_string(String *s);

#endif // !UTILS_H
