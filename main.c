#include <stdio.h>
#include <string.h>
#include "help.h"
#include "op.h"
#include "util.h"

int main(int argn, char** argv) {
    if (argn < 2) {
        display_manual();
        return 0;
    }
    
    Request request = {0};
    int last = argn - 1;
    for(int i = 1; i < argn; i++) {
        char* arg = argv[i];
        if(is_flag(arg)) {
            if (is_help(arg)) {
                display_manual();
                return 0;
            }
            if (is_alias(arg)) {
                if (last <= i) {
                    return error_no_more_data();
                }
                arg = argv[++i];
                if (is_flag(arg)) {
                    return error_invalid_data();
                }
                request.alias = arg;
            } else if (is_database(arg)) {
                if (last <= i) {
                    return error_no_more_data();
                }
                arg = argv[++i];
                if (is_flag(arg)) {
                    return error_invalid_data();
                }
                request.database = arg;
            } else {
                Operation op = is_operation(arg);

                switch(op) {
                    case None:
                        return error_unknown_flag();
                    case ChangeAlias:
                        if (last < i + 2) {
                            return error_no_more_data();
                        }
                        request.alias = argv[++i];
                    default:
                        if (request.operation != None) {
                            return error_multiple_ops();
                        }
                        request.operation = op;
                        if (last > i && is_flag(argv[i + 1]) == 0) {
                            request.data = argv[++i];
                        }
                }
            }
        } else {
            // not a flag
            return error_dangling_data(arg);
        }
    }
    return process_request(request);
}

