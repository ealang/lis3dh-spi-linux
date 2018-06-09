#include <stdint.h>

struct Accel3 {
  float x, y, z;
};

struct Lis3dhStatus {
  // data is not being read fast enough
  int overrun;
  // data is available to read
  int data_available;
};

#define LIS3DH_SAMPLE_RATE_1HZ    0b1
#define LIS3DH_SAMPLE_RATE_10HZ   0b10
#define LIS3DH_SAMPLE_RATE_25HZ   0b11
#define LIS3DH_SAMPLE_RATE_50HZ   0b100
#define LIS3DH_SAMPLE_RATE_100HZ  0b101
#define LIS3DH_SAMPLE_RATE_200HZ  0b110
#define LIS3DH_SAMPLE_RATE_400HZ  0b111

/**
 * Write values to the lis3dh registers to initialize it.
 */
uint8_t lis3dh_initialize(int fd, int selectPin, uint8_t sample_rate_flags);

/**
 * Test if we can communicate with the lis3dh.
 */
void lis3dh_self_check(int fd, int selectPin);

/**
 * Check data status of the device.
 */
struct Lis3dhStatus lis3dh_status(int fd, int selectPin);

/**
 * Take an acceleration sample.
 */
struct Accel3 lis3dh_sample_accel(int fd, int selectPin);
