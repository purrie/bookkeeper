#ifndef UTIL_H_
#define UTIL_H_
#include <stdio.h>
#include "op.h"

int is_flag(char* arg);
int is_alias(char* arg);
int is_help(char* arg);
int is_database(char* arg);
String get_database_folder();
String get_database_path(char* name);
Database get_database(char* name, char* mode);
String read_line(Database* fs);
int break_line(String line, String* result);
void remove_line(Database* db, size_t index);
void trim(char* text);

#endif // UTIL_H_
