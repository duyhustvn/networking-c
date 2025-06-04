#ifndef DECODE_H_
#define DECODE_H_

/* pcap provides this, but we don't want to depend on libpcap */
#ifndef DLT_EN10MB
#define DLT_EN10MB 1
#endif

/* taken from pcap's bpf.h */
#ifndef DLT_RAW
#ifdef __OpenBSD__
#define DLT_RAW     14  /* raw IP */
#else
#define DLT_RAW     12  /* raw IP */
#endif
#endif

#define LINKTYPE_ETHERNET    DLT_EN10MB
#define LINKTYPE_RAW         DLT_RAW

#endif // DECODE_H_
