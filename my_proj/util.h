#ifndef UTIL_H_
#define UTIL_H_

void decimal_to_ascii(int value, char *numArray);
void hex_to_ascii(int value, char *numArray);
void print_string(char *data, ...);
void print_char(char data);
void clear_string(char *cmdStringPtr, int iterations, int start);
int exponent(int number, int exp);
int string_to_number(char *str, int base);

#endif