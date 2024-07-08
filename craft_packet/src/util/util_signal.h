#ifndef UTIL_SIGNAL_H_
#define UTIL_SIGNAL_H_

#include <signal.h>

void utilSignalHandlerSetup(int, void (*handler)(int));

#endif // UTIL_SIGNAL_H_
