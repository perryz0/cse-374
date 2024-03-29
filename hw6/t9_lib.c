// Name: Perry Chien
// Date: 2/19/24
// CSE 374 Homework 6: T9 Implementation
/*
    This is a program that simulates the functionalities of a T9 predictive
    text system. It provides functions for initializing an empty T9 dictionary,
    loading words from a file into the dictionary, adding words to dictionary,
    predicting words based on T9 number sequences, and destroying a dictionary
    to free memory. The implementation utilizes a trie data structure to store
    and retrieve words based on their T9 representations. It also checks input
    validity to ensure only valid T9 key sequences are processed.
*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t9_lib.h"
#include "t9_priv.h"


// HELPER FUNCTION/PROTOTYPE DECLARATIONS (with comments)

// Helps translate letters to corresponding keypad digits. Returns a digit
// between 2 and 9 inclusive if the letter is a valid lowercase English
// letter. Otherwise, a value of -1 is returned, indicating invalid input.
// Parameters:
//   - letter: Letter (part of a word) being translated into T9 number digit
int charToDigit(char letter);

// Helps initialize an empty children array of number nodes.
// Parameters:
//   - parent: Pointer to the T9 node whose child number nodes are initialized
void initializeChildNodes(T9* parent);

// Security check for memory allocation. If unable to allocate storage, then
// exits the program with stderr message.
// Parameters:
//   - ptr: Pointer to the supposedly allocated region of memory
void mallocCheck(void* ptr);


// FUNCTION IMPLEMENTATIONS (comments & pre-/post- conditions in header file)

T9* InitializeEmptyT9() {
    T9* dict = (T9*)malloc(sizeof(T9));
    // Check for malloc failure possibility
    mallocCheck(dict);

    // Initialize root node so it holds no word and no garbage values
    dict->currWord = NULL;

    // Initialize empty children for new root node
    initializeChildNodes(dict);

    // To initialize an empty linked list
    dict->nextWord = NULL;
    return dict;
}

T9* InitializeFromFileT9(const char* filename) {
    // Create empty T9 dictionary to start initialization
    T9* dict = InitializeEmptyT9();

    // Open new file stream to initialize file
    FILE* file = fopen(filename, "r");

    // Handles non-existent file error
    if (!file) {
        perror("Error: ");
        return NULL;
    }

    // Max word length 50 and +1 for null terminator
    char word[MAX_WORD_LENGTH];
    // Iterate through each line (i.e. 1 line = 1 word) in the file
    while (fgets(word, sizeof(word), file) != NULL) {
        int length = strlen(word) + 1;
        // Remove newline character for each line of word in file
        if (word[length - 2] == '\n') {
            word[length - 2] = '\0';
            length--;
        }
        // Check if word exceeds maximum length
        if (length > MAX_WORD_LENGTH) {
            fprintf(stderr, "Word '%s' exceeds maximum length. Skipping.\n",
                                                                        word);
            continue;
        }
        AddWordToT9(dict, word);
    }

    // Dealloc file stream after use
    fclose(file);
    return dict;
}

void AddWordToT9(T9* dict, const char* word) {
    // Check if the word here is NULL or empty
    if (word == NULL || word[0] == 0) {
        return;
    }

    // Start traversal from the root
    T9* current = dict;

    // Trie traversal until null terminator is hit
    for (int i = 0; word[i] != 0; ++i) {
        // Initialize current keypad digit
        int digit = charToDigit(word[i]);
        int child_index = digit - 2;

        // Check validity of the T9 digit
        if (digit == -1) {
            return;
        }

        // Checks if the number digit exists within the child or not
        if (!(current->children[child_index])) {
            // Allocates memory for the new number digit
            current->children[child_index] = (T9*)malloc(sizeof(T9));

            // Check for malloc failure possibility
            mallocCheck(current->children[child_index]);

            // Initialize keypad digit children to be empty
            initializeChildNodes(current->children[child_index]);

            // Assign no word/linked list stored at the new number digit
            current->children[child_index]->currWord = NULL;
            current->children[child_index]->nextWord = NULL;
        }

        // Traverses current to the child node that matches the digit
        current = current->children[child_index];
    }

    // Post-traversal: First check if word already exists in dictionary
    if (current->currWord == NULL) {
        // Allocates memory for the newly stored word
        current->currWord = (char*)malloc((strlen(word) + 1));

        // Check for malloc failure possibility
        mallocCheck(current->currWord);

        // Adds the new word to T9 dictionary
        strncpy(current->currWord, word, strlen(word) + 1);
    } else {
        while (current->nextWord != NULL) {
            // Traverse down the linked list
            if (strncmp(current->currWord, word, strlen(word)) == 0) {
                // If the word already exists, exit the function
                return;
            }
            current = current->nextWord;
        }

        // Check the last word in the linked list
        if (strncmp(current->currWord, word, strlen(word)) == 0) {
            // If the word already exists, exit the function
            return;
        }

        // Initialize, allocate memory, and add new word to end of linked list
        // Coupled with malloc functionality security checks
        current->nextWord = (T9*)malloc(sizeof(T9));
        mallocCheck(current->nextWord);
        current->nextWord->currWord = (char*)malloc((strlen(word) + 1));
        mallocCheck(current->nextWord->currWord);
        strncpy(current->nextWord->currWord, word, strlen(word) + 1);

        // Assign null linked list and child number nodes to the new word
        initializeChildNodes(current->nextWord);
        current->nextWord->nextWord = NULL;
    }
}

char* PredictT9(T9* dict, const char* nums) {
    // Handles case when nums is NULL
    if (nums == NULL) {
        return NULL;
    }

    // Starts traversal from the root
    T9* current = dict;
    int numPounds = 0;

    // Handles invalid input where '#' is at the beginning
    if (nums[0] == '#') {
        return NULL;
    }
    // Iterates through each digit of the number sequence pointed by nums
    for (int i = 0; nums[i] != 0; i++) {
        if (nums[i] == '#') {
            numPounds++;
        } else {
            // Handles invalid input where digits come after first '#'
            if (numPounds != 0) {
                return NULL;
            }

            // Extracts current digit value
            int digit = nums[i] - '0';
            int child_index = digit - 2;

            // Checks if the digit is a valid number between 2 and 9 inclusive
            if (digit < 2 || digit > 9) {
                return NULL;
            }

            // Checks existing num sequences thus far in T9 dictionary
            if (current->children[child_index] == NULL) {
                return NULL;
            }

            // Move down the trie to next number digit
            current = current->children[child_index];
        }
    }

    // Traverse through the linked list if input has pound signs
    while (numPounds > 0 && current != NULL && current->nextWord != NULL) {
        current = current->nextWord;
        numPounds--;
    }

    // Return the word if found, otherwise return NULL
    return (numPounds == 0) ? current->currWord : NULL;
}

void DestroyT9(T9* dict) {
    // Base case: nothing to destroy
    if (dict == NULL) {
        return;
    }

    // Recursively traverse through the Trie and destroy child nodes
    for (int i = 0; i < MAX_CHILDREN; i++) {
        DestroyT9(dict->children[i]);
    }

    // Destroy linked list of "#" key stored nodes
    T9* current = dict->nextWord;
    while (current != NULL) {
        T9* temp = current;
        current = current->nextWord;
        free(temp->currWord);
        free(temp);
    }

    // Free memory for the current node
    free(dict->currWord);
    free(dict);
}



// HELPER FUNCTION DEFINITIONS (comments above in declarations)

int charToDigit(char letter) {
    if (letter >= 'a' && letter <= 'c') return 2;
    if (letter >= 'd' && letter <= 'f') return 3;
    if (letter >= 'g' && letter <= 'i') return 4;
    if (letter >= 'j' && letter <= 'l') return 5;
    if (letter >= 'm' && letter <= 'o') return 6;
    if (letter >= 'p' && letter <= 's') return 7;
    if (letter >= 't' && letter <= 'v') return 8;
    if (letter >= 'w' && letter <= 'z') return 9;
    return -1;
}

void initializeChildNodes(T9* parent) {
    for (int i = 0; i < MAX_CHILDREN; ++i) {
        parent->children[i] = NULL;
    }
}

void mallocCheck(void* ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }
}
