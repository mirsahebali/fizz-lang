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

// clone the data from `value`
// WARN: cleanup value if not using it anymore
// may lead to mem leak
String str_clone(String *value);

// moves the data from `src` and frees up src;
String str_move(String *src);

char char_at_str(String *, int32_t);

bool cmp_str(String *left, String *right);

bool is_letter(char);

bool is_digit(char);

unsigned char count_digits(int input);

// Always call this after reassigning strings or being done after use
void free_string(String *s);

typedef struct {
  String *data;
  int32_t capacity;
  int32_t size;
} StringArray;

StringArray string_array_init(int32_t capacity);
int32_t string_array_push(StringArray *self, String);
int32_t string_array_size(const StringArray *self);
bool string_array_reserve(StringArray *self);
int32_t string_array_capacity(const StringArray *);
String string_array_get(const StringArray *self, int32_t index);
void print_string_array(const StringArray *);
void free_string_array(StringArray *self);

#define ASSERT_STR(val) #val

#define ASSERT_EQ(left, right, message)                                        \
  if (left != right) {                                                         \
    printf("%s\n", message);                                                   \
    printf("left = %f\n", left);                                               \
    printf("right = %f\n", right);                                             \
    assert(left == right);                                                     \
  }

#endif // !UTILS_H
