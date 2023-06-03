#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include "op.h"
#include "help.h"
#include "util.h"

int process_add(char* data, char* alias, char* database) {
    if (data == NULL) {
        fprintf(stderr, "ERROR: need to provided data to be inserted");
        return -1;
    }
    Database db = get_database(database, "a+");
    rewind(db.file);
    // find all aliases to see if we have a conflict
    // seek all the entries to see if we have a duplicate
    size_t line_number = 0;
    while (!feof(db.file)) {
        String line = read_line(&db);
        String parts[2] = {0};
        if (break_line(line, parts)) {
            // no alias
            if (strcmp(data, line.buffer) == 0) {
                if (alias != NULL) {
                    fprintf(stderr, "Data '%s' without alias exists, adding alias", data);
                    goto replace;
                }
                // the same, skipping adding duplicate
                fprintf(stderr, "Element '%s' already exists in %s database\n", data, database);
                goto end;
            }
        } else {
            // field with alias
            int same_data = strcmp(data, parts[1].buffer) == 0;
            int same_alias = alias == NULL ? 0 : strcmp(alias, parts[0].buffer) == 0;
            if (same_alias || same_data) {
                fprintf(stderr, "Element exists, updating\n");
                goto replace;
            }
        }
        line_number ++;
    }

    add:
    if (alias == NULL) {
        fputs(data, db.file);
    } else {
        fputs(alias, db.file);
        fputs("$", db.file);
        fputs(data, db.file);
    }
    fputs("\n", db.file);

    end:
    if (ferror(db.file)) {
        fprintf(stderr, "ERROR: %s", strerror(errno));
        fclose(db.file);
        return -1;
    }
    fclose(db.file);
    return 0;

    replace:
    remove_line(&db, line_number);
    fseek(db.file, 0, SEEK_END);
    goto add;
}

int process_get(char* index, char* alias, char* database) {
    if (index == NULL && alias == NULL) {
        return error_invalid_data();
    }
    Database db = get_database(database, "r");
    size_t number = index == NULL ? 0 : strlen(index);
    if (alias == NULL) {
        if (number == 1 && index[0] == '0') {
            number = 0;
        } else {
            number = atol(index);
            if (number == 0) {
                fclose(db.file);
                fprintf(stderr, "ERROR: Index isn't a valid number");
                return -1;
            }
        }
    }

    while (!feof(db.file)) {
        String line = read_line(&db);
        if (alias != NULL) {
            String parts[2] = {0};
            if (break_line(line, parts) == 0) {
                if (strcmp(alias, parts[0].buffer) == 0) {
                    fclose(db.file);
                    printf("%s", parts[1].buffer);
                    return 0;
                }
            }
        } else {
            if (number == 0) {
                String parts[2] = {0};
                if (break_line(line, parts)) {
                    printf("%s", line.buffer);
                    fclose(db.file);
                    return 0;
                } else {
                    printf("%s", parts[1].buffer);
                    fclose(db.file);
                    return 0;
                }
            }
            number--;
        }
    }

    fclose(db.file);
    if (alias != NULL) {
        fprintf(stderr, "ERROR: Alias not found");
    } else {
        fprintf(stderr, "ERROR: Index out of bounds");
    }
    return -1;
}

int process_remove(char* index, char* alias, char* database) {
    if (index == NULL && alias == NULL) {
        return error_invalid_data();
    }

    Database db = get_database(database, "a+");
    rewind(db.file);
    if (alias == NULL) {
        size_t len = strlen(index);
        if (len == 1 && index[0] == '0') {
            remove_line(&db, 0);
            goto success;
        } else {
            size_t line = atol(index);
            if (line == 0) {
                fclose(db.file);
                fprintf(stderr, "ERROR: Failed to convert provided index to a number");
                exit(-1);
            }
            remove_line(&db, line);
            goto success;
        }
    } else {
        size_t it = 0;
        while(!feof(db.file)) {
            String s = read_line(&db);
            String parts[2] = {0};
            if (break_line(s, parts) == 0) {
                if (strcmp(parts[0].buffer, alias) == 0) {
                    remove_line(&db, it);
                    goto success;
                }
            }
            it ++;
        }
    }
    fclose(db.file);
    return 1;
    success:
    fclose(db.file);
    return 0;
}

int process_delete_database(char* name) {
    String path = get_database_path(name);
    if(remove(path.buffer)) {
        fprintf(stderr, "%s", strerror(errno));
        return errno;
    }
    return 0;
}

int process_count_elements(char* database) {
    Database db = get_database(database, "r");

    size_t len = 0;
    while(!feof(db.file)) {
        String s = read_line(&db);
        if (strlen(s.buffer) > 0)
            len ++;
    }
    printf("%zu", len);
    fclose(db.file);
    return 0;
}

int process_get_alias(char* index, char* database) {
    Database db = get_database(database, "r");

    size_t number = strlen(index);
    if (number == 0) {
        fprintf(stderr, "ERROR: No index provided");
        fclose(db.file);
        return -1;
    }

    if (number != 1 || index[0] != '0') {
        number = atol(index);
        if (number == 0) {
            fclose(db.file);
            fprintf(stderr,
                    "ERROR: Failed to convert provided index to a number");
            return -1;
        }
    }

    for (int i = 0; i < number; i++) {
        read_line(&db);
        if (feof(db.file)) {
            fprintf(stderr, "ERROR: Index is out of bounds");
            fclose(db.file);
            return -1;
        }
    }
    String line = read_line(&db);
    fclose(db.file);

    String parts[2] = {0};
    if (break_line(line, parts) == 0) {
        printf("%s", parts[0].buffer);
    }
    return -1;
}

int process_get_alias_list(char* database) {
    Database db = get_database(database, "r");
    char ret = -1;

    while (!feof(db.file)) {
        String line = read_line(&db);
        String parts[2] = {0};
        if (break_line(line, parts) == 0) {
            if (ret == 0) {
                printf("\n%s", parts[0].buffer);
            } else {
                printf("%s", parts[0].buffer);
            }
            ret = 0;
        }
    }
    fclose(db.file);
    return ret;
}

int process_get_databases() {
    String folder = get_database_folder();
    DIR* dir = opendir(folder.buffer);
    if (dir == NULL) {
        fprintf(stderr, "ERROR: %s", strerror(errno));
        return errno;
    }
    struct dirent* el = readdir(dir);
    char first = 1;
    while (el != NULL) {
        if (el->d_type != DT_REG) {
            goto next;
        }
        size_t len = strlen(el->d_name);
        char* ext = el->d_name + len - 3;
        if (strcmp(ext, ".db") != 0) {
            goto next;
        }
        char name[STRING_BUFFER_SIZE] = {0};
        strcpy(name, el->d_name);
        memset(name + len - 3, 0, sizeof(char) * 3);
        if (first) {
            first = 0;
            printf("%s", name);
        } else {
            printf("\n%s", name);
        }

        next:
        el = readdir(dir);
    }

    closedir(dir);
    return 0;
}

int process_change_alias(char* alias, char* index, char* database) {
    if (alias == NULL || index == NULL) {
        fprintf(stderr, "ERROR: Changing alias requires both alias and indx");
        return -1;
    }
    size_t number = strlen(index);
    if (number == 1 && index[0] == '0') {
        number = 0;
    } else {
        number = atol(index);
        if (number == 0) {
            fprintf(stderr, "ERROR: Provided index is not a number");
            return -1;
        }
    }
    size_t it = number;
    Database db = get_database(database, "a+");
    rewind(db.file);

    while(!feof(db.file) && it != 0) {
        read_line(&db);
        it--;
    }
    if (it > 0) {
        fprintf(stderr, "ERROR: Index out of bounds");
        fclose(db.file);
        return -1;
    }
    String line = read_line(&db);
    remove_line(&db, number);
    char new_line[STRING_BUFFER_SIZE] = {0};
    if (strcmp(alias, " ") != 0) {
        strcpy(new_line, alias);
        strcat(new_line, "$");
    }
    String parts[2] = {0};
    if (break_line(line, parts)) {
        strcat(new_line, line.buffer);
    } else {
        strcat(new_line, parts[1].buffer);
    }
    fseek(db.file, 0, SEEK_END);
    if (fputs(new_line, db.file) < 0) {
        fprintf(stderr, "ERROR: Failed to write to database");
        fclose(db.file);
        return -1;
    }
    fclose(db.file);
    return 0;
}

int process_query_data_exists(char* data, char* database) {
    if (data == NULL) {
        fprintf(stderr, "ERROR: Need to provide data");
        return -1;
    }
    Database db = get_database(database, "r");
    while(!feof(db.file)) {
        String line = read_line(&db);
        String parts[2] = {0};
        if (break_line(line, parts)) {
            if (strcmp(data, line.buffer) == 0) {
                goto success;
            }
        } else {
            if (strcmp(parts[1].buffer, data) == 0) {
                goto success;
            }
        }
    }

    fclose(db.file);
    return 1;
success:
    fclose(db.file);
    return 0;
}

int process_request(Request request) {
    if (request.database == NULL) {
        request.database = "global";
    }
    if (request.alias != NULL) {
        trim(request.alias);
    }
    if (request.data != NULL) {
        trim(request.data);
    }
    switch (request.operation) {
    case None:
        fprintf(stderr, "ERROR: No operation declared");
        return 1;
    case Add:
        return process_add(request.data, request.alias, request.database);
    case Get:
        return process_get(request.data, request.alias, request.database);
    case Remove:
        return process_remove(request.data, request.alias, request.database);
    case DeleteDatabase:
        return process_delete_database(request.database);
    case Count:
        return process_count_elements(request.database);
    case GetAlias:
        return process_get_alias(request.data, request.database);
    case GetAliasList:
        return process_get_alias_list(request.database);
    case GetDatabases:
        return process_get_databases();
    case ChangeAlias:
        return process_change_alias(request.alias, request.data,
                                    request.database);
    case QueryDataExists:
        return process_query_data_exists(request.data, request.database);
    }
    fprintf(stderr, "ERROR: Unhandled operation");
    return -1;
}

Operation is_operation(char* arg) {
    if (strlen(arg) > 2) {
        return None;
    }
    if (arg[0] != '-') {
        return None;
    }
    if (arg[1] == 'i') return Add;
    if (arg[1] == 'o') return Get;
    if (arg[1] == 'r') return Remove;
    if (arg[1] == 'R') return DeleteDatabase;
    if (arg[1] == 'c') return Count;
    if (arg[1] == 'C') return GetAliasList;
    if (arg[1] == 'D') return GetDatabases;
    if (arg[1] == 'A') return ChangeAlias;
    if (arg[1] == 'q') return QueryDataExists;

    return None;
}
