#include <stdio.h>
#include <string.h>
#include "process.h"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) return show_help_invalid();

    char* first = argv[1];
    char* second = argv[2];
    int result = 0;

    if ( (strcmp(first, "on") == 0) && ! second ) result = on_runner();
    else if ( (strcmp(first, "off") == 0) && ! second) result = off_runner();
    else if ( (strcmp(first, "status") == 0) && ! second) result = show_status();
    else if ( (strcmp(first, "env") == 0) && (strcmp(second, "--add") == 0) ) result = add_env();
    else if ( (strcmp(first, "env") == 0) && (strcmp(second, "--remove") == 0) ) result = remove_env();
    else if ( (strcmp(first, "registry") == 0) && (strcmp(second, "--add") == 0) ) result = add_registry();
    else if ( (strcmp(first, "registry") == 0) && (strcmp(second, "--remove") == 0) ) result = remove_registry();
    else if ( (strcmp(first, "--version") == 0) && ! second) result = show_version();
    else if ( (strcmp(first, "--help") == 0) && ! second) result = show_help();
    else result = show_help_invalid();

    return result == 0 ? 0 : 1;
}
