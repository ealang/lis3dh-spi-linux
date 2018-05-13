/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <unistd.h>

#include "lis3dh-registers.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
  perror(s);
  abort();
}

static const char *device = "/dev/spidev0.0";
static uint8_t mode = SPI_CPHA | SPI_CPOL;
static uint8_t bits = 8;
static uint32_t speed = 100000;
static uint16_t delay;

#define DIVISOR_2G_SCALE_HR  ((((1 << 15) - 1) & ~0xF) >> 1)

static void transaction(int fd, uint8_t *tx, uint32_t size, uint8_t *rx)
{
  int ret;
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)tx,
    .rx_buf = (unsigned long)rx,
    .len = size,
    .delay_usecs = delay,
    .speed_hz = speed,
    .bits_per_word = bits,
    .cs_change = 0
  };

  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1)
    pabort("can't send spi message");
}

static void hex_dump(uint8_t *data, uint32_t size) {
uint32_t i;
  for (i = 0; i < size; i++) {
    if (!(i % 6))
      puts("");
    printf("%.2X ", data[i]);
  }
  puts("");
}

static void write_reg(int fd, uint8_t reg, uint8_t value) {
  uint8_t rx[2];
  uint8_t tx[2] = {reg, value};
  transaction(fd, tx, 2, rx);
}

static uint8_t read_reg_8(int fd, uint8_t reg) {
  uint8_t rx[2] = {0, 0};
  uint8_t tx[] = {
    reg | 0x80, 0
  };
  transaction(fd, tx, 2, rx);
  return rx[1];
}

static uint16_t read_reg_16(int fd, uint8_t reg) {
  uint8_t rx[3] = {0, 0, 0};
  uint8_t tx[] = {
    reg | 0x80 | 0x40, 0, 0
  };
  transaction(fd, tx, 3, rx);
  return (uint16_t)rx[1] | (uint16_t)rx[2] << 8;
}

static uint8_t who_am_i(int fd)
{
  return read_reg_8(fd, LIS3DH_WHO_AM_I);
}

static uint8_t status(int fd)
{
  return read_reg_8(fd, LIS3DH_STATUS_REG2);
}

static float accel_x(int fd) {
  return (float)((int16_t)read_reg_16(fd, LIS3DH_OUT_X_L)) / DIVISOR_2G_SCALE_HR;
}

static float accel_y(int fd) {
  return (float)((int16_t)read_reg_16(fd, LIS3DH_OUT_Y_L)) / DIVISOR_2G_SCALE_HR;
}

static float accel_z(int fd) {
  return (float)((int16_t)read_reg_16(fd, LIS3DH_OUT_Z_L)) / DIVISOR_2G_SCALE_HR;
}

static uint8_t configure(int fd) {
  // REG1
  uint8_t ORD = 2; // sample rate
  uint8_t EN = 7; // enable all axes
  uint8_t reg_1_val = ORD << 4 | EN;
  write_reg(fd, LIS3DH_CTRL_REG1, reg_1_val);

  usleep(1000 * 100);

  uint8_t BLOCK_DATA_UPDATE = 0;
  uint8_t SCALE = 0; // 2g
  uint8_t HIGH_RES = 1; // enable high res
  uint8_t reg_4_val = SCALE << 4 | HIGH_RES << 3 | BLOCK_DATA_UPDATE;
  write_reg(fd, LIS3DH_CTRL_REG4, reg_4_val);
}

static void print_usage(const char *prog)
{
  printf("Usage: %s [-DsbdlHOLC3]\n", prog);
  puts("  -D --device   device to use (default /dev/spidev0.0)\n"
       "  -s --speed    max speed (Hz)\n"
       "  -d --delay    delay (usec)\n"
       "  -b --bpw      bits per word \n"
       "  -l --loop     loopback\n"
       "  -H --cpha     clock phase\n"
       "  -O --cpol     clock polarity\n"
       "  -L --lsb      least significant bit first\n"
       "  -C --cs-high  chip select active high\n"
       "  -3 --3wire    SI/SO signals shared\n");
  exit(1);
}

static void parse_opts(int argc, char *argv[])
{
  while (1) {
    static const struct option lopts[] = {
      { "device",  1, 0, 'D' },
      { "speed",   1, 0, 's' },
      { "delay",   1, 0, 'd' },
      { "bpw",     1, 0, 'b' },
      { "loop",    0, 0, 'l' },
      { "cpha",    0, 0, 'H' },
      { "cpol",    0, 0, 'O' },
      { "lsb",     0, 0, 'L' },
      { "cs-high", 0, 0, 'C' },
      { "3wire",   0, 0, '3' },
      { "no-cs",   0, 0, 'N' },
      { "ready",   0, 0, 'R' },
      { NULL, 0, 0, 0 },
    };
    int c;

    c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);

    if (c == -1)
      break;

    switch (c) {
    case 'D':
      device = optarg;
      break;
    case 's':
      speed = atoi(optarg);
      break;
    case 'd':
      delay = atoi(optarg);
      break;
    case 'b':
      bits = atoi(optarg);
      break;
    case 'l':
      mode |= SPI_LOOP;
      break;
    case 'H':
      mode |= SPI_CPHA;
      break;
    case 'O':
      mode |= SPI_CPOL;
      break;
    case 'L':
      mode |= SPI_LSB_FIRST;
      break;
    case 'C':
      mode |= SPI_CS_HIGH;
      break;
    case '3':
      mode |= SPI_3WIRE;
      break;
    case 'N':
      mode |= SPI_NO_CS;
      break;
    case 'R':
      mode |= SPI_READY;
      break;
    default:
      print_usage(argv[0]);
      break;
    }
  }
}

int main(int argc, char *argv[])
{
  int ret = 0;
  int fd;

  parse_opts(argc, argv);

  fd = open(device, O_RDWR);
  if (fd < 0)
    pabort("can't open device");

  /*
   * spi mode
   */
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if (ret == -1)
    pabort("can't set spi mode");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if (ret == -1)
    pabort("can't get spi mode");

  /*
   * bits per word
   */
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
  if (ret == -1)
    pabort("can't get bits per word");

  /*
   * max speed hz
   */
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't set max speed hz");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if (ret == -1)
    pabort("can't get max speed hz");



  printf("spi mode: %d\n", mode);
  printf("bits per word: %d\n", bits);
  printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);

  usleep(1000 * 500);
  printf("%x\n", who_am_i(fd));
  usleep(1000 * 500);
  configure(fd);


  usleep(1000 * 500);
  printf("%x: %x %x\n", who_am_i(fd), read_reg_8(fd, LIS3DH_CTRL_REG1), read_reg_8(fd, LIS3DH_CTRL_REG4));

  setbuf(stdout, NULL);

  int i;
  for (i = 0; i < 1000; i++) {
    uint8_t s = status(fd);
    if (s & 0xF0) {
      printf("%x: %x\n", who_am_i(fd), s);
    }
    printf(".");
    if (s & 8) {
      printf("%c[2K\r%f %f %f\n", 27, accel_x(fd), accel_y(fd), accel_z(fd));
    }
    usleep(1000 * 50);
  }
  close(fd);

  return ret;
}
