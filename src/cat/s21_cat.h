#ifndef S21_CAT_H
#define S21_CAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_FILE_NOT_FOUND \
  "s21_cat: Error: File '%s' does not exist or cannot be opened.\n"
#define ERROR_INVALID_FLAG "s21_cat: Error: Invalid option '%c'.\n"

typedef struct {
  int flag_b;
  int flag_e;
  int flag_n;
  int flag_s;
  int flag_t;
  int flag_v;
  int new_line_flag;
  int line_counter_b;
  int squeeze_counter_s;
  int is_empty_line;
  int file_count;
} s21_cat_flags_t;

int parse_flags(int argc, char **argv, s21_cat_flags_t *flags);
FILE *process_file(FILE *file, s21_cat_flags_t *flags);
void print_usage();

#endif  // S21_CAT_H
