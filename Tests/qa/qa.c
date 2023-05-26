#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdbm.h>

#define MAX_ANSWERS 10
#define MAX_STRING_SIZE 100

typedef struct {
    char string1[MAX_STRING_SIZE];
    char string2[MAX_STRING_SIZE];
} Answer;

typedef struct {
    Answer answers[MAX_ANSWERS];
    char additionalString[MAX_STRING_SIZE];
} Question;

void serializeQuestion(const Question* question, char* buffer) {
    int i;
    char* current = buffer;

    for (i = 0; i < MAX_ANSWERS; i++) {
        memcpy(current, &question->answers[i], sizeof(Answer));
        current += sizeof(Answer);
    }

    memcpy(current, question->additionalString, sizeof(question->additionalString));
}

void deserializeQuestion(const char* buffer, Question* question) {
    int i;
    const char* current = buffer;

    for (i = 0; i < MAX_ANSWERS; i++) {
        memcpy(&question->answers[i], current, sizeof(Answer));
        current += sizeof(Answer);
    }

    memcpy(question->additionalString, current, sizeof(question->additionalString));
}

int main() {
    // Open the GDBM database
    GDBM_FILE db = gdbm_open("mydatabase.db", 0, GDBM_WRCREAT, 0666, NULL);

    if (!db) {
        fprintf(stderr, "Failed to open database.\n");
        exit(1);
    }

    // Create a Question struct instance
    Question question;

    // Set values for the first Answer
    strcpy(question.answers[0].string1, "Hello");
    strcpy(question.answers[0].string2, "World");

    // Set values for the second Answer
    strcpy(question.answers[1].string1, "Goodbye");
    strcpy(question.answers[1].string2, "Universe");

    // Set value for the additional string
    strcpy(question.additionalString, "This is additional data.");

    // Calculate the size required for serialization
    size_t bufferSize = MAX_ANSWERS * sizeof(Answer) + sizeof(question.additionalString);
    char serialized[bufferSize];

    // Serialize the Question struct into a byte array
    serializeQuestion(&question, serialized);

    // Store the serialized Question struct in GDBM using a key
    datum key, value;
    key.dptr = "question1";
    key.dsize = strlen(key.dptr) + 1;
    value.dptr = serialized;
    value.dsize = bufferSize;

    if (gdbm_store(db, key, value, GDBM_REPLACE) != 0) {
        fprintf(stderr, "Failed to store struct in database.\n");
        exit(1);
    }

    // Retrieve the serialized Question struct from GDBM
    key.dptr = "question1";
    key.dsize = strlen(key.dptr) + 1;
    value = gdbm_fetch(db, key);

    if (value.dptr) {
        // Deserialize the byte array back into a Question struct
        Question retrievedQuestion;
        deserializeQuestion(value.dptr, &retrievedQuestion);

        // Print the retrieved Question struct
        printf("First Answer - String 1: %s\n", retrievedQuestion.answers[0].string1);
        printf("First Answer - String 2: %s\n", retrievedQuestion.answers[0].string2);
        printf("Second Answer - String 1: %s\n", retrievedQuestion.answers[1].string1);
        printf("Second Answer - String 2: %s\n", retrievedQuestion.answers[1].string2);
        printf("Additional String: %s\n", retrievedQuestion.additionalString);

        // Free the allocated memory
        free(value.dptr);
    } else {
        fprintf(stderr, "Failed to retrieve struct from database.\n");
    }

    // Close the GDBM database
    gdbm_close(db);

    return 0;
}