#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>

#include "pabort.h"
#include "lis3dh-spi-dev.h"

int open_spi_device_as_lis3dh(const char *device_name, uint32_t speed_hz) {
  int fd = open(device_name, O_RDWR);
  if (fd < 0)
    pabort("can't open device");

  uint8_t mode = SPI_CPHA | SPI_CPOL;
  int ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
    pabort("can't set spi mode");

  uint8_t bits = 8;
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz);
  if (ret == -1)
    pabort("can't set max speed hz");

  return fd;
}

void close_spi_device(int fd) {
  close(fd);
}
