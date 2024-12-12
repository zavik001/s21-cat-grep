#include "s21_cat.h"

int main(int argc, char **argv) {
  if (argc > 1) {
    s21_cat_flags_t flags = {0};
    if (parse_flags(argc, argv, &flags)) {
      for (int i = (argc - flags.file_count); i < argc; i++) {
        FILE *file = fopen(argv[i], "r");
        if (file != NULL) {
          fclose(process_file(file, &flags));
        } else {
          fprintf(stderr, ERROR_FILE_NOT_FOUND, argv[i]);
        }
      }
    }
  } else {
    fprintf(stderr, "s21_cat: Error: No files provided.\n");
    print_usage();
  }
  return 0;
}

void print_usage() {
  printf("Usage: s21_cat [OPTION]... [FILE]...\n");
  printf("Concatenate FILE(s) to standard output.\n\n");
  printf("Options:\n");
  printf("  -b, --num-nonblank       Number non-blank lines.\n");
  printf(
      "  -e                       Display $ at the end of each line (implies "
      "-v).\n");
  printf("  -E                       Display $ at the end of each line.\n");
  printf("  -n, --number             Number all lines.\n");
  printf("  -s, --squeeze-blank      Squeeze multiple blank lines into one.\n");
  printf("  -t                       Display tabs as ^I (implies -v).\n");
  printf("  -T                       Display tabs as ^I.\n");
  printf(
      "  -v                       Use visible mode for non-printable "
      "characters.\n");
  printf("\n");
  printf("Examples:\n");
  printf(
      "  s21_cat file.txt           Output contents of file.txt to standard "
      "output.\n");
  printf(
      "  s21_cat -n file1 file2     Output file1 and file2 with line "
      "numbers.\n");
}

int parse_flags(int argc, char **argv, s21_cat_flags_t *flags) {
  int file_index_start = 1, status = 1, i = 1;

  for (; (i < argc && argv[i][0] == '-'); file_index_start = (i += 1)) {
    if (!strcmp(argv[i], "-b") || !strcmp(argv[i], "--num-nonblank")) {
      flags->flag_b = (flags->flag_n = 0) + 1;
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--squeeze-blank")) {
      flags->flag_s = 1;
    } else if (!strcmp(argv[i], "-n") || !strcmp(argv[i], "--number")) {
      flags->flag_n = flags->flag_b ? 0 : 1;
    } else if (!strcmp(argv[i], "-T")) {
      flags->flag_t = 1;
    } else if (!strcmp(argv[i], "-v")) {
      flags->flag_v = 1;
    } else if (!strcmp(argv[i], "-t")) {
      flags->flag_t = 1;
      flags->flag_v = 1;
    } else if (!strcmp(argv[i], "-e")) {
      flags->flag_e = 1;
      flags->flag_v = 1;
    } else if (!strcmp(argv[i], "-E")) {
      flags->flag_e = 1;
    } else {
      fprintf(stderr, ERROR_INVALID_FLAG, argv[i][1]);
      print_usage();
      status = 0;
    }
  }

  flags->file_count = argc - file_index_start;

  return status;
}

FILE *process_file(FILE *file, s21_cat_flags_t *flags) {
  for (char sym = '0'; (sym = getc(file)) != EOF;) {
    flags->is_empty_line = 0;
    if (flags->flag_s && flags->squeeze_counter_s == 0 && sym == '\n') {
      flags->squeeze_counter_s += 1;
    } else if (flags->squeeze_counter_s != 0 && sym == '\n') {
      flags->squeeze_counter_s += 1;
      flags->is_empty_line = 1;
    } else if (flags->squeeze_counter_s > 1 && sym != '\n') {
      flags->squeeze_counter_s = 0;
      flags->flag_e ? printf("$\n") : printf("\n");
      if (flags->flag_n != 0) printf("%6d\t", flags->flag_n++);
    } else {
      flags->squeeze_counter_s = 0;
    }
    if (flags->flag_n != 0 || flags->flag_b != 0) {
      if (flags->new_line_flag == 1 && !(flags->new_line_flag = 0))
        printf("%6d\t", flags->flag_n++);
      if (flags->flag_n == 1) printf("%6d\t", flags->flag_n++);
      if (flags->flag_b == 1) printf("%6d\t", flags->flag_b++);
      if (sym == '\n' && flags->flag_n != 0 && flags->is_empty_line == 0)
        flags->new_line_flag = 1;
      if (sym == '\n' && flags->flag_b != 0) flags->line_counter_b += 1;
      if (sym != '\n' && flags->line_counter_b != 0 &&
          flags->squeeze_counter_s == 0)
        if (!(flags->line_counter_b = 0)) printf("%6d\t", flags->flag_b++);
    }
    if (sym == '\n' && flags->flag_e && flags->is_empty_line == 0) printf("$");
    if (flags->flag_v) {
      if (sym < 32 && sym != 9 && sym != 10)
        if (sym += 64) printf("^");
      if (sym == 127)
        if ((sym = '?')) printf("^");
    }
    if (flags->flag_t && sym == '\t')
      if ((sym = 'I')) printf("^");
    if (flags->is_empty_line == 0) printf("%c", sym);
  }
  return file;
}
