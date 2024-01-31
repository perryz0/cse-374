// Name: Perry Chien
// Date: 1/28/24
// CSE 374 Homework 3: Wordcount
/*
    This is a program that simulates the unix command "wc". It reads in one or
    more files and report statistics (i.e. number of lines, words, and chars)
    for the file(s). When an option is flagged (e.g. "-l") and indicated as
    the 2nd argument, only the specified statistic (i.e. lines) is printed.
    Otherwise, if no option is specified, all three statistics will be shown
    and labeled next to each of the listed file names, alongside an additional
    total line count for all of the files. Unless the passed arguments are
    invalid (i.e. incorrect usage of the program), counts will be provided,
    even if none of the files exist (i.e. total lines = 0).
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Max compatible line length of input file
#define MAXLINE 500



// FUNCTION DECLARATIONS (with comments)

// Processes a single input file and calculates the number of lines, words, and
// characters based on the specified option (i.e. -l, -w, and -c respectively).
// If no option is specified, the total number of lines are printed.
// Parameters:
//   - filename: Name of current input file to be processed.
//   - lineTotal: Pointer to the tracker for total # of lines across files.
//   - lineBoolean: Boolean that indicates whether to print # of lines.
//   - wordBoolean: Boolean that indicates whether to print # of words.
//   - charBoolean: Boolean that indicates whether to print # of characters.
void fileProcessor(char* filename, int *lineTotal, int lineBoolean,
                                            int wordBoolean, int charBoolean);

// Helps with file processing by iterating through each line and count values.
// Parameters:
//   - currFile: Pointer to the input file
//   - wordBoolean: Boolean that indicates whether to print # of words.
//   - charBoolean: Boolean that indicates whether to print # of characters.
//   - lines: Pointer to variable that stores # of lines in current file
//   - words: Pointer to variable that stores # of words in current file
//   - chars: Pointer to variable that stores # of lines in current file
void processHelper(FILE *currFile, int wordBoolean, int charBoolean,
                                        int *lines, int *words, int *chars);

// Helps with printing the counted line, word, and char values based on the
// specified options.
// Parameters:
//   - filename: Name of current input file to be processed.
//   - lineBoolean: Boolean that indicates whether to print # of lines.
//   - wordBoolean: Boolean that indicates whether to print # of words.
//   - charBoolean: Boolean that indicates whether to print # of characters.
//   - lines: # of lines in current file.
//   - words: # of words in current file.
//   - chars: # of lines in current file.
void printHelper(char* filename, int lineBoolean, int wordBoolean,
                            int charBoolean, int lines, int words, int chars);

// Prints the total number of lines for all files.
// Parameters:
//   - lineTotal: Total # of lines across files.
//   - lineBoolean: Boolean that indicates whether to print # of lines.
//   - wordBoolean: Boolean that indicates whether to print # of words.
//   - charBoolean: Boolean that indicates whether to print # of characters.
void totalLines(int lineTotal, int lineBoolean, int wordBoolean,
                                                        int charBoolean);



// FUNCION DEFINITIONS

int main(int argc, char* argv[]) {
    // Handles error of no input files passed as argument.
    if (argc < 2 || (argc == 2 && argv[1][0] == '-' && strlen(argv[1]) == 2
         && (argv[1][1] == 'l' || argv[1][1] == 'w' || argv[1][1] == 'c'))) {
        fprintf(stderr, "Usage: ./wordcount requires an input file.\n");
        return EXIT_FAILURE;
    }

    // Tracks total number of lines
    int lineTotal = 0;

    // Options tracker (i.e. decides which values out of the three to print)
    int lineBoolean = 1, wordBoolean = 1, charBoolean = 1;

    // Process all options/flags
    for (int i = 1; i < argc; i++) {
        if (i == 1 && strncmp((argv[i]), "-l", 3) == 0) {
            wordBoolean = 0, charBoolean = 0;
        } else if (i == 1 && strncmp((argv[i]), "-w", 3) == 0) {
            lineBoolean = 0, charBoolean = 0;
        } else if (i == 1 && strncmp((argv[i]), "-c", 3) == 0) {
            lineBoolean = 0, wordBoolean = 0;
        } else {
            // Process each individual file passed as argument
            fileProcessor(argv[i], &lineTotal, lineBoolean, wordBoolean,
                                                                charBoolean);
        }
    }

    // Checker and printer for total lines
    totalLines(lineTotal, lineBoolean, wordBoolean, charBoolean);

    return EXIT_SUCCESS;
}



/* Non-main helper functions; all descriptions above in the declarations. */

void fileProcessor(char* filename, int *lineTotal, int lineBoolean,
                                            int wordBoolean, int charBoolean) {
    // Opens current file in read mode and initializes counters
    FILE *currFile = fopen(filename, "r");
    int lines = 0, words = 0, chars = 0;

    // Checks whether file failed to open
    if (currFile == NULL) {
        fprintf(stderr, "%s will not open. Skipping.\n", filename);
        return;
    }

    // Iterate through each line and performs counting
    processHelper(currFile, wordBoolean, charBoolean, &lines, &words, &chars);

    // Closes current file stream
    fclose(currFile);

    // Print the desired values based on options
    printHelper(filename, lineBoolean, wordBoolean, charBoolean, lines,
                                                                words, chars);

    // Update total line value across all files
    *lineTotal += lines;
}

void processHelper(FILE *currFile, int wordBoolean, int charBoolean,
                                        int *lines, int *words, int *chars) {
    char arr[MAXLINE];
    while (fgets(arr, MAXLINE, currFile) != NULL) {
        (*lines)++;
        if (charBoolean) {
            (*chars) += strlen(arr);
        }
        if (wordBoolean) {
            char *token = strtok(arr, " \n");
            while (token != NULL) {
                (*words)++;
                token = strtok(NULL, " \n");
            }
        }
    }
}

void printHelper(char* filename, int lineBoolean, int wordBoolean,
                        int charBoolean, int lines, int words, int chars) {
    if (lineBoolean && wordBoolean && charBoolean) {
        printf("%d %d %d %s\n", lines, words, chars, filename);
    } else {
        if (lineBoolean) {
            printf("%d\n", lines);
        }

        if (wordBoolean) {
            printf("%d\n", words);
        }

        if (charBoolean) {
            printf("%d\n", chars);
        }
    }
}

void totalLines(int lineTotal, int lineBoolean, int wordBoolean,
                                                    int charBoolean) {
    if (lineBoolean && wordBoolean && charBoolean) {
        printf("Total Lines = %d\n", lineTotal);
    }
}
