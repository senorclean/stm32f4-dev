#ifndef UTIL_H_
#define UTIL_H_

void decimal_to_ascii(int value, char *numArray);
void hex_to_ascii(int value, char *numArray);
void reverse_array(char *str);
void print_string(char *data, ...);
void print_char(char data);
void clear_string(char *cmdStringPtr, int iterations);

#endif