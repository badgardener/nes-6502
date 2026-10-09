/**
 * Work only when compiled C23+
 * And "../assets/nestest-real.log" is present.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 65536
#define CONTEXT_BEFORE 10
#define CONTEXT_AFTER 30

static const unsigned char expected_log[] = {
#embed "../assets/nestest-real.log"
};

static void print_context(const char *label, const char *line, size_t length,
                          size_t first_diff) {
  size_t start = first_diff > CONTEXT_BEFORE ? first_diff - CONTEXT_BEFORE : 0;
  size_t end = first_diff + CONTEXT_AFTER;

  if (end > length)
    end = length;

  printf("  %-9s: ", label);
  fwrite(line + start, 1, end - start, stdout);
  putchar('\n');

  if (strcmp(label, "REAL") == 0) {
    printf("            ");
    for (size_t i = start; i < first_diff; i++)
      putchar(' ');
    puts("^");
  }
}

static size_t trim_newline(char *line, size_t length) {
  while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r'))
    line[--length] = '\0';

  return length;
}

static int read_line(FILE *file, char **buffer, size_t *capacity,
                     size_t *length) {
  size_t used = 0;
  int ch;

  while ((ch = fgetc(file)) != EOF) {
    if (used + 1 >= *capacity) {
      if (*capacity > (size_t)-1 / 2)
        return -1;

      size_t new_capacity = *capacity * 2;
      char *new_buffer = realloc(*buffer, new_capacity);

      if (!new_buffer)
        return -1;

      *buffer = new_buffer;
      *capacity = new_capacity;
    }

    (*buffer)[used++] = (char)ch;

    if (ch == '\n')
      break;
  }

  if (ferror(file))
    return -1;

  if (used == 0 && ch == EOF)
    return 0;

  (*buffer)[used] = '\0';
  *length = trim_newline(*buffer, used);

  return 1;
}

static int read_embedded_line(const unsigned char **cursor,
                              const unsigned char *end, char **buffer,
                              size_t *capacity, size_t *length) {
  if (*cursor >= end)
    return 0;

  size_t used = 0;

  while (*cursor < end) {
    unsigned char ch = *(*cursor)++;

    if (used + 1 >= *capacity) {
      if (*capacity > (size_t)-1 / 2)
        return -1;

      size_t new_capacity = *capacity * 2;
      char *new_buffer = realloc(*buffer, new_capacity);

      if (!new_buffer)
        return -1;

      *buffer = new_buffer;
      *capacity = new_capacity;
    }

    (*buffer)[used++] = (char)ch;

    if (ch == '\n')
      break;
  }

  (*buffer)[used] = '\0';
  *length = trim_newline(*buffer, used);

  return 1;
}

static void compare_files(const char *real_path) {
  FILE *real_file = fopen(real_path, "rb");

  if (!real_file) {
    perror(real_path);
    exit(EXIT_FAILURE);
  }

  size_t real_capacity = LINE_BUFFER_SIZE;
  size_t expected_capacity = LINE_BUFFER_SIZE;
  size_t real_length = 0;
  size_t expected_length = 0;

  char *real_line = malloc(real_capacity);
  char *expected_line = malloc(expected_capacity);

  if (!real_line || !expected_line) {
    fprintf(stderr, "Error: memory allocation failed\n");
    free(real_line);
    free(expected_line);
    fclose(real_file);
    exit(EXIT_FAILURE);
  }

  const unsigned char *cursor = expected_log;
  const unsigned char *end = expected_log + sizeof(expected_log);

  size_t total = 0;
  size_t identical = 0;
  size_t different = 0;

  puts("======================================================================="
       "=========");
  puts("NES CPU TRACE COMPARISON");
  puts("======================================================================="
       "=========");
  printf("REAL     : %s\n", real_path);
  printf("EXPECTED : ../assets/log (embedded)\n");
  puts("======================================================================="
       "=========");

  for (;;) {
    int real_status =
        read_line(real_file, &real_line, &real_capacity, &real_length);

    int expected_status = read_embedded_line(
        &cursor, end, &expected_line, &expected_capacity, &expected_length);

    if (real_status < 0 || expected_status < 0) {
      fprintf(stderr, "\nError: failed to read trace or allocate memory\n");
      free(real_line);
      free(expected_line);
      fclose(real_file);
      exit(EXIT_FAILURE);
    }

    if (real_status == 0 && expected_status == 0)
      break;

    total++;

    if (real_status == 0) {
      different++;
      printf("\n[MISSING IN REAL] Line %zu\n", total);
      printf("  EXPECTED: %s\n", expected_line);
      continue;
    }

    if (expected_status == 0) {
      different++;
      printf("\n[MISSING IN EXPECTED] Line %zu\n", total);
      printf("  REAL    : %s\n", real_line);
      continue;
    }

    if (real_length == expected_length &&
        memcmp(real_line, expected_line, real_length) == 0) {
      identical++;
      continue;
    }

    different++;

    printf("\n[MISMATCH] Line %zu\n", total);
    printf("  REAL    : %s\n", real_line);
    printf("  EXPECTED: %s\n", expected_line);

    size_t max_length =
        real_length > expected_length ? real_length : expected_length;
    size_t first_diff = 0;

    while (first_diff < max_length && first_diff < real_length &&
           first_diff < expected_length &&
           real_line[first_diff] == expected_line[first_diff])
      first_diff++;

    printf("  First differing column: %zu\n", first_diff + 1);

    print_context("REAL", real_line, real_length, first_diff);
    print_context("EXPECTED", expected_line, expected_length, first_diff);
  }

  puts("\n====================================================================="
       "===========");
  puts("COMPARISON SUMMARY");
  puts("======================================================================="
       "=========");
  printf("Total lines compared : %zu\n", total);
  printf("Identical lines      : %zu\n", identical);
  printf("Different lines      : %zu\n", different);
  printf("Result               : %s\n", different == 0 ? "MATCH" : "MISMATCH");
  puts("======================================================================="
       "=========");

  free(real_line);
  free(expected_line);
  fclose(real_file);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <real-trace.log>\n", argv[0]);
    return 2;
  }

  compare_files(argv[1]);
  return 0;
}
