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

#define __USE_GNU

enum {
    RESULT_OK = 0,
    RESULT_FAILURE,
};

#endif // COMMON_H_
