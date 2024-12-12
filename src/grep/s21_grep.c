#include "s21_grep.h"

int main(int argc, char *argv[]) {
  grep_flags_t flags = {0};
  int result = 1;

  if (parse_flags(argc, argv, &flags)) {
    process_files(argc, argv, &flags);
    result = 0;
  } else {
    print_usage();
  }

  return result;
}

int parse_flags(int argc, char *argv[], grep_flags_t *flags) {
  int opt, success = 1;

  while ((opt = getopt(argc, argv, "e:ivcln")) != -1) {
    switch (opt) {
      case 'e':
        flags->flag_e = 1;
        if (flags->pattern_count < 256) {
          strncpy(flags->patterns[flags->pattern_count++], optarg,
                  MAX_LINE_LEN - 1);
        }
        break;
      case 'i':
        flags->flag_i = 1;
        break;
      case 'v':
        flags->flag_v = 1;
        break;
      case 'c':
        flags->flag_c = 1;
        break;
      case 'l':
        flags->flag_l = 1;
        break;
      case 'n':
        flags->flag_n = 1;
        break;
      default:
        success = 0;
        break;
    }
  }

  if (flags->pattern_count == 0 && optind < argc) {
    strncpy(flags->patterns[flags->pattern_count++], argv[optind++],
            MAX_LINE_LEN - 1);
  }

  if (flags->pattern_count == 0) success = 0;

  if (flags->flag_l && flags->flag_c) flags->flag_c = 0;

  return success;
}

void process_files(int argc, char *argv[], grep_flags_t *flags) {
  int start_index = optind >= argc ? argc : optind;

  for (int i = start_index; i < argc; i++) {
    process_file(argv[i], flags);
  }
}

int process_file(const char *filename, grep_flags_t *flags) {
  FILE *file = open_file(filename);
  if (!file) return 0;

  regex_t regex[flags->pattern_count];
  int result = compile_patterns(regex, flags);

  if (result) {
    process_lines(file, regex, flags, filename);
  }

  for (int i = 0; i < flags->pattern_count; i++) {
    regfree(&regex[i]);
  }

  if (file != stdin) fclose(file);

  return result;
}

FILE *open_file(const char *filename) {
  FILE *file = (filename) ? fopen(filename, "r") : stdin;
  if (!file && filename) perror(filename);
  return file;
}

int compile_patterns(regex_t *regex, grep_flags_t *flags) {
  int reg_flags = REG_EXTENDED | (flags->flag_i ? REG_ICASE : 0);
  int success = 1;

  for (int i = 0; i < flags->pattern_count && success; i++) {
    if (regcomp(&regex[i], flags->patterns[i], reg_flags) != 0) {
      fprintf(stderr, "Error compiling regex: %s\n", flags->patterns[i]);
      success = 0;
    }
  }

  return success;
}

void process_lines(FILE *file, regex_t *regex, grep_flags_t *flags,
                   const char *filename) {
  char line[MAX_LINE_LEN];
  int line_number = 0, match_count = 0, matched_file = 0;

  while (fgets(line, sizeof(line), file)) {
    line_number++;
    int match = match_line(line, regex, flags);

    if (flags->flag_v) match = !match;

    if (match) {
      matched_file = 1;
      match_count++;
      if (!flags->flag_c && !flags->flag_l) {
        print_line(line, filename, flags, line_number);
      }
    }
  }

  if (flags->flag_l && matched_file) {
    printf("%s\n", filename);
  } else if (flags->flag_c && !flags->flag_l) {
    print_count(filename, match_count);
  }
}

int match_line(const char *line, regex_t *regex, grep_flags_t *flags) {
  int is_match = 0;

  for (int i = 0; i < flags->pattern_count && !is_match; i++) {
    if (regexec(&regex[i], line, 0, NULL, 0) == 0) {
      is_match = 1;
    }
  }

  return is_match;
}

void print_line(const char *line, const char *filename, grep_flags_t *flags,
                int line_number) {
  if (filename) printf("%s:", filename);
  if (flags->flag_n) printf("%d:", line_number);
  printf("%s", line);
}

void print_count(const char *filename, int count) {
  if (filename) printf("%s:", filename);
  printf("%d\n", count);
}

void print_usage() {
  printf("Usage: s21_grep [options] pattern [file...]\n");
  printf("Options:\n");
  printf(
      "  -e pattern   Use pattern for matching (can be used multiple times)\n");
  printf("  -i           Ignore case distinctions\n");
  printf("  -v           Invert match\n");
  printf("  -c           Count matching lines\n");
  printf("  -l           Print only filenames with matches\n");
  printf("  -n           Prefix each line with its line number\n");
}
