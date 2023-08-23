#include "utils.h"

unsigned int conv2BytesToInt(const unsigned char *p) {
  unsigned int i = p[0];
  return i << 8 | p[1];
}

unsigned int conv4BytesToInt(const unsigned char *p) {
  unsigned int res = p[0];

  res = res << 8 | p[1];
  res = res << 8 | p[2];
  res = res << 8 | p[3];

  return res;
};
