#ifndef HELP_H_
#define HELP_H_

void display_manual();
int error_no_more_data();
int error_invalid_data();
int error_unknown_flag();
int error_multiple_ops();
int error_dangling_data(char* arg);

#endif // HELP_H_
