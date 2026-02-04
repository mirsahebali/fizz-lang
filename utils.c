#include "utils.h"

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
