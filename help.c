#include <stdio.h>
#include "help.h"

void display_manual() {
    printf("BookKeeper, simple information database\n");
    printf("Usage: bookkeeper [FLAG]... [DATA]\n");
    printf("  -i [DATA]           Save [DATA] to global database\n");
    printf("  -o [INDEX]          Retrieve data from database at [INDEX]\n");
    printf("                        [INDEX] must be a number 0 or greater\n");
    printf("                        if [INDEX] is greater than number of entries\n");
    printf("                        the program exits with non-zero exit code\n");
    printf("  -r [INDEX]          Removes entry at specified index from the global database\n");
    printf("  -R                  Removes every entry from the database, deleting it completely\n");
    printf("  -d [DATABASE]       Use database by name [DATABASE] instead of global one\n");
    printf("                        if no relevant flag is used with -d, an empty database is created\n");
    printf("  -a [ALIAS]          When used with -i flag, the entry is saved with specified alias\n");
    printf("                        when used with -o flag, the entry is retrieved by alias instead of index\n");
    printf("                        when used with -r flag, the entry is removed by alias instead of index\n");
    printf("                        If alias is not found, it fallbacks to index,\n");
    printf("                        if that is not provided, the program exits with non-zero exit code\n");
    printf("  -A [ALIAS] [INDEX]  Change alias for the field at specified index\n");
    printf("  -n [INDEX]          Retrieve an alias for the field at [INDEX]\n");
    printf("                        empty string is returned if the field doesn't have an alias\n");
    printf("  -c                  Retrieve number of entries in the global database\n");
    printf("  -C                  Retrieve a list of aliases from the database that can be used to index it\n");
    printf("                        The list is delimited by a (\\n) new line character\n");
    printf("                        Fields that don't have an alias are omited\n");
    printf("  -D                  Retrieve list of databases, including global one\n");
    printf("                        The list is delimited by a (\\n) new line character\n");
    printf("  -q [DATA]           Tests whatever the data exists within the database, returns nonzero if it doesn't\n");
    printf("  -h, --help          Display this help document and exit\n\n");

    printf("Example:\n");
    printf("  bookkeeper -i abcd\n");
    printf("    Adds abcd to the global database\n\n");
    printf("  bookkeeper -o 0\n");
    printf("    Returns the first element from the global database\n\n");
    printf("  bookkeeper -A 0 number\n");
    printf("    Assigns alias 'number' to the first element of the global database\n\n");
    printf("  bookkeeper -i 12345 -a password -d passwords\n");
    printf("    Adds number '12345' to database named 'passwords' with alias 'password'\n\n");
    printf("  bookkeeper -o -a password -d passwords\n");
    printf("    Returns a value with alias 'password' from database named 'passwords'\n\n");
    printf("  bookkeeper -R -d passwords\n");
    printf("    Deletes 'passwords' database\n\n");

    printf("Copyright © 2023 Purrie Brightstar.\n\
   License  GPLv3: GNU GPL version 3\n\
   <https://gnu.org/licenses/gpl.html>.\n\
   This is free software: you are free to change and redistribute it.\n\
   There is NO WARRANTY, to the extent permitted by law.");
}

int error_no_more_data() {
    fprintf(stderr, "ERROR: Program flag expected data but none was provided");
    return 2;
}
int error_invalid_data() {
    fprintf(stderr, "ERROR: Program flag expected data but received another flag");
    return 3;
}

int error_unknown_flag() {
    fprintf(stderr, "ERROR: Unrecognized flag");
    return 4;
}

int error_multiple_ops() {
    fprintf(stderr, "ERROR: Multiple operations requested");
    return 5;
}

int error_dangling_data(char* arg) {
    fprintf(stderr, "ERROR: Provided data '%s' without a flag", arg);
    return 6;
}
