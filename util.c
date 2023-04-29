#include <sys/stat.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "op.h"


int is_flag(char* arg) {
    if (strlen(arg) > 3) {
        return 0;
    }
    return arg[0] == '-';
}

int is_alias(char* arg) {
    return strcmp(arg, "-a") == 0;
}

int is_help(char* arg) {
    return strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0;
}

int is_database(char* arg) {
    return strcmp(arg, "-d") == 0;
}

String get_database_folder() {
    String path = {0};
    char* home = getenv("XDG_DATA_HOME");
    if (home == NULL) {
        home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "Could not find home folder for current user");
            exit(-1);
        }
        strcat(path.buffer, home);
        strcat(path.buffer, "/.local/share/bookkeeper/");
    } else {
        strcat(path.buffer, home);
        strcat(path.buffer, "/bookkeeper/");
    }
    if (mkdir(path.buffer, 0700)) {
        if (errno != EEXIST) {
            fprintf(stderr, "%s", strerror(errno));
            exit(-1);
        }
    }
    return path;
}

String get_database_path(char* name) {
    String path = get_database_folder();
    strcat(path.buffer, name);
    strcat(path.buffer, ".db");
    return path;
}

Database get_database(char* name, char* mode) {
    Database db = {0};
    if (name == NULL) {
        return db;
    }
    String path = get_database_path(name);
    db.file = fopen(path.buffer, mode);
    if (db.file == NULL) {
        fprintf(stderr, "ERROR: Failed to open database %s: %s", name,  strerror(errno));
        exit(errno);
    }
    strcpy(db.path, path.buffer);
    strcpy(db.mode, mode);
    return db;
}

String read_line(Database* db) {
    String result = {0};
    if (fgets(result.buffer, STRING_BUFFER_SIZE - 1, db->file) == NULL) {
        if (ferror(db->file)) {
            fprintf(stderr, "ERROR: Reading line failed because of: %s", strerror(errno));
            fclose(db->file);
            exit(-1);
        }
    }
    // remove new line character
    size_t last = strlen(result.buffer) - 1;
    if (result.buffer[last] == '\n')
        result.buffer[last] = '\0';
    return result;
}

int break_line(String line, String* result) {
    for (int i = 0; i < STRING_BUFFER_SIZE; i++) {
        if (line.buffer[i] == '$') {
            int l = strlen(line.buffer);
            memcpy(result[0].buffer, line.buffer, i);
            memcpy(result[1].buffer, line.buffer + i + 1, l - i - 1);
            return 0;
        }
    }
    return 1;
}

size_t count_lines(Database* db) {
    long line = ftell(db->file);
    rewind(db->file);
    size_t count = 0;
    String s = {0};
    while (!feof(db->file)) {
        if (fgets(s.buffer, STRING_BUFFER_SIZE, db->file) != NULL) {
            count ++;
        } else if (ferror(db->file)) {
            fclose(db->file);
            fprintf(stderr, "%s", strerror(errno));
            exit(-1);
        }
    }
    fseek(db->file, line, SEEK_SET);
    return count;
}

void remove_line(Database* db, size_t index) {
    assert(db->mode[1] == '+');
    rewind(db->file);
    size_t count = count_lines(db);
    if (index >= count) {
        fprintf(stderr, "ERROR: Requested removal of item outside the bounds of the database\n");
        fclose(db->file);
        exit(-1);
    }

    String lines[count - 1];

    int it = 0;
    for(int i = 0; i < count; i++) {
        String line = read_line(db);
        if (strlen(line.buffer) == 0) {
            if (feof(db->file)) {
              fprintf(stderr,
                      "ERROR: Encountered end of file before it was expected");
              fclose(db->file);
              exit(-1);
            }
        }
        if (i != index) {
            lines[it] = line;
            it++;
        }
    }
    fclose(db->file);
    if(remove(db->path)) {
        fprintf(stderr, "ERROR: Failed to remove old database because of: %s", strerror(errno));
        exit(-1);
    }

    db->file = fopen(db->path, db->mode);
    if (db->file == NULL) {
        fprintf(stderr, "ERROR: Failed to recreate a new database because of: %s", strerror(errno));
        exit(-1);
    }

    for(int i = 0; i < count - 1; i++) {
        if (fputs(lines[i].buffer, db->file) < 0) {
            goto error;
        }
        if (fputs("\n", db->file) < 0) {
            goto error;
        }
    }

    return;

    error:
    fclose(db->file);
    fprintf(stderr, "ERROR: Couldn't update database because of: %s", strerror(errno));
    exit(-1);
}

void trim(char* text) {
    size_t len = strlen(text);
    while (len --> 0) {
        if (text[len] == ' ') {
            text[len] = '\0';
        } else {
            break;
        }
    }
    size_t start = 0;
    while (start < len) {
        if (text[start] != ' ') {
            break;
        }
        start ++;
    }
    if (start > 0) {
        memmove(text, text + start, sizeof(char) * (len - start + 1));
        memset(text + len - start + 1, 0, start);
    }
}
