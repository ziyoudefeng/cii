
//<includes 4>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

//<data 4>
int linenum;

//<prototypes 4>
int getword(FILE *fp, char *buf, int size);
void doubleword(char *name, FILE *fp);

//<functions 3>
int main(int argc, char *argv[])
{
    int i;

    for (i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: can't open '%s' (%s)\n", argv[0], argv[i], strerror(errno));
            return EXIT_FAILURE;
        } else {
            doubleword(argv[i], fp);
            fclose(fp);
        }
    }
    //if (argc == 1) doubleword(NULL, stdin);
    FILE *fp = fopen("test.txt", "r");
    doubleword("test.txt", fp);
    return EXIT_SUCCESS;
}

int getword(FILE *fp, char *buf, int size)
{
    int c;

    c = getc(fp);
    //<scan forward to a nonspace character or EOF 5>
    for (; c != EOF && isspace(c); c = getc(fp)) {
        if (c == '\n') {
            linenum++;
        }
    }

    //<copy the word into buf[0..size-1] 5>
    int i = 0;
    for (; c != EOF && !isspace(c); c = getc(fp)) {
        if (i < size-1) {
            buf[i++] = tolower(c);
        }
    }
    if (i < size) {
        buf[i] = '\0';
    }
    if (c != EOF) {
        ungetc(c, fp);
    }


    //return <found a word? 5>
    return buf[0] != '\0';
}


void doubleword(char *name, FILE *fp)
{
    char prev[12], word[12];

    linenum = 1;
    prev[0] = '\0';
    while (getword(fp, word, sizeof(word))) {
        if (isalpha(word[0]) && strcmp(prev, word) == 0) {
            //<word is a duplicate 6>
            if (name) {
                printf("%s: ", name);
            }
            printf("%d: %s\n", linenum, word);
        }
        strcpy(prev, word);
    }
}
