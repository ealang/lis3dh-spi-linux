#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

#include "src/lis3dh-spi-dev.h"
#include "src/lis3dh.h"
#include "src/pabort.h"

static const int rpiPin3 = 8;
static const int rpiPin5 = 9;

static void setupGPIO() {
  if (wiringPiSetup() == -1) {
    pabort("unable to setup wiringPI");
  }
  pinMode(rpiPin3, OUTPUT);
  pinMode(rpiPin5, OUTPUT);
  digitalWrite(rpiPin3, 1);
  digitalWrite(rpiPin5, 1);
}

int main(int argc, char *argv[]) {

  uint32_t spi_speed_hz = 1000 * 1000;
  const char *device = "/dev/spidev0.0";
  uint8_t sample_rate_flag = LIS3DH_SAMPLE_RATE_10HZ;
  uint32_t poll_interval_us = 50 * 1000;
  int selectPin = rpiPin3;

  setupGPIO();

  int fd = open_spi_device_as_lis3dh(device, spi_speed_hz);
  lis3dh_self_check(fd, selectPin);
  lis3dh_initialize(fd, selectPin, sample_rate_flag);

  int first_poll = 1;
  while (1) {
    struct Lis3dhStatus status = lis3dh_status(fd, selectPin);
    if (status.overrun && !first_poll) {
      fprintf(stderr, "overrun encountered\n");
    }

    if (status.data_available) {
      struct Accel3 accel = lis3dh_sample_accel(fd, selectPin);
      printf("%f, %f, %f\n", accel.x, accel.y, accel.z);
    }

    usleep(poll_interval_us);
    first_poll = 0;
  }
  close_spi_device(fd);

  return 0;
}
