#include <stdint.h>

struct Args {
  int isValid; 
  uint32_t selectPin;
  uint32_t sampleRateFlag;
  uint32_t pollIntervalUs;
};

struct Args parseArgs(int arglen, char *argc[]);
