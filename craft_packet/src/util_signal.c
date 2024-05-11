#include "util_signal.h"

void utilSignalHandlerSetup(int sig, void (*handler)(int)) {
    signal(sig, handler);
};
