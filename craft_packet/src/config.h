#ifndef CONFIG_H_
#define CONFIG_H_

typedef struct config_ {
    char *srcIP;
    char *srcMac;
    char *dstMac;
    char *deviceInterface;
    char *filePath;
    int   timeout;
} config;

#endif // CONFIG_H_
