#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#include <stdbool.h>

bool is_letter(char);

bool is_digit(char);

unsigned char count_digits(int input);

#define ASSERT_STR(val) #val

#define ASSERT_EQ(left, right, message)                                        \
  if (left != right) {                                                         \
    printf("%s\n", message);                                                   \
    printf("left = %f\n", left);                                               \
    printf("right = %f\n", right);                                             \
    assert(left == right);                                                     \
  }

#define NOT_IMPLEMENTED(msg)                                                   \
  printf("Not implemented: %s %s:%d\n", __FUNCTION__, __FILE_NAME__,           \
         __LINE__);                                                            \
  printf("Message: %s", msg);                                                  \
  exit(1);

#endif // !UTILS_H
