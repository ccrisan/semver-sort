
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "semver.h"

#define MAX_LEN 4097


bool quiet = false;
bool reverse = false;
bool help = false;
bool only_ver = false;
char sep = ' ';
int field = 1;


int compare_func(const void *pindex1, const void *pindex2, void *data) {
    semver_t *semvers = data;
    int index1 = *((int *) pindex1);
    int index2 = *((int *) pindex2);
    
    int result = semver_compare(semvers[index1], semvers[index2]);
    if (reverse) {
        result *= -1;
    }
    
    return result;
}

int main(int argc, char *argv[]) {
    char line[MAX_LEN], c, *s, *t, *line_copy, **lines = NULL;
    int i, len, count = 0, *indexes;
    semver_t *semvers = NULL, semver;

    /* Parse command line args */
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-q")) {
            quiet = true;
        }
        else if (!strcmp(argv[i], "-r")) {
            reverse = true;
        }
        else if (!strcmp(argv[i], "-h")) {
            help = true;
        }
        else if (!strcmp(argv[i], "-v")) {
            only_ver = true;
        }
        else if (!strcmp(argv[i], "-t")) {
            if (argc <= i + 1) {
                break;
            }

            sep = argv[i++ + 1][0];
        }
        else if (!strcmp(argv[i], "-k")) {
            if (argc <= i + 1) {
                help = true;
                break;
            }

            field = strtol(argv[i++ + 1], NULL, 10);
            if (field < 1) {
                field = 1;
            }
        }
        else {
            help = true;
        }
    }

    if (help) {
        printf("Usage: <versions> | semver-sort [options]\n");
        printf("Sort all semver-valid versions at input and write them at output.\n");
        printf("Available options:\n");
        printf("\t-h         show this help\n");
        printf("\t-k <num>   extract and use field <num> instead of all line (starts at 1)\n");
        printf("\t-r         sort descending\n");
        printf("\t-t <char>  use <char> as field separator (defaults to space)\n");
        printf("\t-q         do not print any messages\n");
        printf("\t-v         print only versions, not entire lines\n");

        return -1;
    }

    while (fgets(line, MAX_LEN, stdin)) {
        len = strlen(line);
        if (len == MAX_LEN && line[len - 1] != '\n') {
            if (!quiet) {
                fprintf(stderr, "Input line too long: %s\n", line);
            }
            continue;
        }

        /* Remove terminal newline */
        line[--len] = 0;
        
        /* Ignore empty lines */
        if (!len) {
            continue;
        }
        
        /* Extract version from line */
        i = 0;
        s = line_copy = strdup(line);
        while (i < field && (t = strsep(&s, &sep))) {
            i++;
        }
        
        if (i < field) {
            if (!quiet) {
                fprintf(stderr, "Field %d not found in %s\n", field, line);
            }
            free(line_copy);
            continue;
        }

        if (semver_parse(t, &semver)) {
            if (!quiet) {
                fprintf(stderr, "Invalid semver: %s\n", line);
            }
            free(line_copy);
            continue;
        }
        
        lines = realloc(lines, sizeof(char *) * (count + 1));
        lines[count] = strdup(only_ver ? t : line);

        semvers = realloc(semvers, sizeof(semver_t) * (count + 1));
        semvers[count++] = semver;

        free(line_copy);
    }
    
    /* Actual sorting */
    indexes = malloc(sizeof(int) * count);    
    for (i = 0; i < count; i++) {
        indexes[i] = i;
    }
    qsort_r(indexes, count, sizeof(int), &compare_func, semvers);
    
    /* Print sorted lines */
    for (i = 0; i < count; i++) {
        printf("%s\n", lines[indexes[i]]);
    }

    /* Free everything */
    for (i = 0; i < count; i++) {
        free(lines[i]);
        semver_free(&semvers[i]);
    }
    free(lines);
    free(semvers);
    free(indexes);

    return 0;
}
