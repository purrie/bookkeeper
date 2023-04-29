#ifndef OP_H_
#define OP_H_
#include <stdio.h>

#ifndef STRING_BUFFER_SIZE
#define STRING_BUFFER_SIZE 255
#endif

#define ARRAY_CAPACITY(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct {
    char buffer[STRING_BUFFER_SIZE];
} String;

typedef enum {
    None = 0, // Default set, for detecting multiple ops or lack thereof
    Add,
    Get,
    Remove,
    DeleteDatabase,
    Count,
    GetAlias,
    GetAliasList,
    GetDatabases,
    ChangeAlias,
    QueryDataExists,
} Operation;

typedef struct {
    Operation operation;
    char* data; // serves as both index and data for respective operations
    char* alias;
    char* database;
} Request;

typedef struct {
    FILE* file;
    char mode[3];
    char path[STRING_BUFFER_SIZE];
} Database;

Operation is_operation(char* arg);
int process_request(Request request);

#endif // OP_H_
