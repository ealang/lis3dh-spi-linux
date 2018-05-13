#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "src/lis3dh-spi-dev.h"
#include "src/lis3dh.h"

int main(int argc, char *argv[]) {

  uint32_t spi_speed_hz = 1000 * 1000;
  const char *device = "/dev/spidev0.0";
  uint8_t sample_rate_flag = LIS3DH_SAMPLE_RATE_10HZ;
  uint32_t poll_interval_us = 50 * 1000;

  int fd = open_spi_device_as_lis3dh(device, spi_speed_hz);
  lis3dh_self_check(fd);
  lis3dh_initialize(fd, sample_rate_flag);

  int first_poll = 1;
  while (1) {
    struct Lis3dhStatus status = lis3dh_status(fd);
    if (status.overrun && !first_poll) {
      fprintf(stderr, "overrun encountered\n");
    }

    if (status.data_available) {
      struct Accel3 accel = lis3dh_sample_accel(fd);
      printf("%f, %f, %f\n", accel.x, accel.y, accel.z);
    }

    usleep(poll_interval_us);
    first_poll = 0;
  }
  close_spi_device(fd);

  return 0;
}
