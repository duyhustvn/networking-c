#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>	
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>	
#include <err.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

enum {
    RESULT_OK = 0,
    RESULT_FAILURE,
};

#endif // COMMON_H_
