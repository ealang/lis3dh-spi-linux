#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <wiringPi.h>

#include "src/lis3dh-spi-dev.h"
#include "src/lis3dh.h"
#include "src/pabort.h"
#include "args.h"

static double ctime_sec() {
  struct timeval t;
  gettimeofday(&t, 0);
  return t.tv_sec + (double)t.tv_usec / 1000000;
}

static void setupGPIO(int selectPin) {
  if (wiringPiSetup() == -1) {
    pabort("unable to setup wiringPI");
  }
  pinMode(selectPin, OUTPUT);
  digitalWrite(selectPin, 1);
}

void run(struct Args args) {
  uint32_t spi_speed_hz = 5 * 1000 * 1000;
  const char *device = "/dev/spidev0.0";

  setupGPIO(args.selectPin);

  int fd = open_spi_device_as_lis3dh(device, spi_speed_hz);
  lis3dh_self_check(fd, args.selectPin);
  lis3dh_initialize(fd, args.selectPin, args.sampleRateFlag);

  double t0 = ctime_sec();
  printf("t,x,y,z\n");
  int first_poll = 1;
  while (1) {
    struct Lis3dhStatus status = lis3dh_status(fd, args.selectPin);
    if (status.overrun && !first_poll) {
      fprintf(stderr, "overrun encountered\n");
    }

    if (status.data_available) {
      struct Accel3 accel = lis3dh_sample_accel(fd, args.selectPin);
      printf("%f,%f,%f,%f\n", ctime_sec() - t0, accel.x, accel.y, accel.z);
    }

    usleep(args.pollIntervalUs);
    first_poll = 0;
  }
  close_spi_device(fd);
}

int main(int arglen, char *argc[]) {
  struct Args args = parseArgs(arglen, argc);
  if (args.isValid) {
    run(args);
  } else {
    fprintf(stderr, "Use:\n");
    fprintf(stderr, "  %s\n", argc[0]);
    fprintf(stderr, "  %s [select-pin] [sample-rate hz {1, 10, 25, 50, 100, 200, 400}]\n", argc[0]);
  }
  return 0;
}
