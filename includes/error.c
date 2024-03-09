#include <stdio.h>
#include <error.h>

#define ERROR_MSG_LEN (sizeof errorMsgs / sizeof(char*))

static const char* errorMsgs[] = {
        NULL,
        "Error encountered while trying to allocate memory.\n",
        "Unexpected end of file encountered.\n",
};

void printErr(Error err) {
    fputs("Error encountered: ", stderr);
    if (err.kind < ERROR_MSG_LEN && errorMsgs[err.kind] != NULL)
        fputs(errorMsgs[err.kind], stderr);
    if (err.msg != NULL)
        fputs(err.msg, stderr);
}
