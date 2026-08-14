#include <unistd.h>
#include <stdlib.h>

#include "common.h"

void lib_fatal(const char *msg, int len) {
    write(STDERR_FILENO, msg, len);
    abort();
}
