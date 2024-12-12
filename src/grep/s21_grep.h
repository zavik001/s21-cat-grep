#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024

typedef struct {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  char patterns[256][MAX_LINE_LEN];
  int pattern_count;
} grep_flags_t;

int parse_flags(int argc, char *argv[], grep_flags_t *flags);
void print_usage();
void process_files(int argc, char *argv[], grep_flags_t *flags);
int process_file(const char *filename, grep_flags_t *flags);
FILE *open_file(const char *filename);
int compile_patterns(regex_t *regex, grep_flags_t *flags);
void process_lines(FILE *file, regex_t *regex, grep_flags_t *flags,
                   const char *filename);
int match_line(const char *line, regex_t *regex, grep_flags_t *flags);
void print_line(const char *line, const char *filename, grep_flags_t *flags,
                int line_number);
void print_count(const char *filename, int count);

#endif  // S21_GREP_H
