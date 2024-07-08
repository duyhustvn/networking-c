#include "util_cpu.h"
#include <stdint.h>
#include <unistd.h>


uint16_t getNumberOfProcessorsOnline() {
    long numCpu = sysconf(_SC_NPROCESSORS_ONLN);
    if (numCpu < 1) {
        return 0;
    }

    return (uint16_t)numCpu;
};
