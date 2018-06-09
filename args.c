#include <stdio.h>
#include <stdlib.h>
#include "src/lis3dh.h"
#include "args.h"

struct Args parseArgs(int arglen, char *argc[]) {
  struct Args ret;
  ret.isValid = 0;

  if (arglen == 3) {
    ret.isValid = 1;
    ret.selectPin = strtol(argc[1], 0, 10);
    int sampleRate = strtol(argc[2], 0, 10);
    switch (sampleRate) {
      case 1:
        ret.sampleRateFlag = LIS3DH_SAMPLE_RATE_1HZ;
        ret.pollIntervalUs = 500 * 1000;
        break;
      case 10:
        ret.sampleRateFlag = LIS3DH_SAMPLE_RATE_10HZ;
        ret.pollIntervalUs = 50 * 1000;
        break;
      case 25:
        ret.sampleRateFlag = LIS3DH_SAMPLE_RATE_25HZ;
        ret.pollIntervalUs = 20 * 1000;
        break;
      case 50:
        ret.sampleRateFlag = LIS3DH_SAMPLE_RATE_50HZ;
        ret.pollIntervalUs = 10 * 1000;
        break;
      case 100:
        ret.sampleRateFlag = LIS3DH_SAMPLE_RATE_100HZ;
        ret.pollIntervalUs = 5 * 1000;
        break;
      case 200:
        ret.sampleRateFlag = LIS3DH_SAMPLE_RATE_200HZ;
        ret.pollIntervalUs = 2 * 1000;
        break;
      case 400:
        ret.sampleRateFlag = LIS3DH_SAMPLE_RATE_400HZ;
        ret.pollIntervalUs = 1 * 1000;
        break;
    }
  } else if (arglen == 1) {
    char *argc2[] = {argc[0], "8", "10"};
    return parseArgs(3, argc2);
  }
  return ret;
}
