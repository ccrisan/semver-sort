
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <semver.h>

#define MAX_LEN 257


bool quiet = false;
bool reverse = false;
bool help = false;


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
    char line[MAX_LEN], **versions = NULL;
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
        else {
            help = true;
        }
    }

    if (help) {
        printf("Usage: <versions> | semver-sort [options]\n");
        printf("Sort all semver-valid versions at input and write them at output.\n");
        printf("Available options:\n");
        printf("\t-h\tshow this help\n");
        printf("\t-r\tsort descending\n");
        printf("\t-q\tdo not print any messages\n");
        
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
        
        if (semver_parse(line, &semver)) {
            if (!quiet) {
                fprintf(stderr, "Invalid semver: %s\n", line);
            }
            continue;
        }

        versions = realloc(versions, sizeof(char *) * (count + 1));
        versions[count] = strdup(line);

        semvers = realloc(semvers, sizeof(semver_t) * (count + 1));
        semvers[count++] = semver;
    }
    
    /* Actual sorting */
    indexes = malloc(sizeof(int) * count);    
    for (i = 0; i < count; i++) {
        indexes[i] = i;
    }
    qsort_r(indexes, count, sizeof(int), &compare_func, semvers);
    
    /* Print sorted versions */
    for (i = 0; i < count; i++) {
        printf("%s\n", versions[indexes[i]]);
    }

    /* Free everything */
    for (i = 0; i < count; i++) {
        free(versions[i]);
        semver_free(&semvers[i]);
    }
    free(versions);
    free(semvers);
    free(indexes);

    return 0;
}
